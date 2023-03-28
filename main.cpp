#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/offboard/offboard.h>
#include <thread>
#include <barrier>
#include <chrono>
#include <future>
#include "lib/flag/flag.hpp"
#include <string>
#include <sstream>
#include "lib/log/log.hpp"

using namespace mavsdk;
using std::vector;
using std::shared_ptr;

// Constants
const std::chrono::seconds max_waiting_time{30};
const float takeoff_altitude{3.0F};
const int64_t refresh_time{1L};
const float reasonable_error{0.3};

// Process return code
enum class ProRetCod : int {
	OK = 0,
	BAD_ARGUMENT = 1,
	CONNECTION_FAILED = 2,
	NO_SYSTEMS_FOUND = 3,
	TELEMETRY_FAILURE = 4,
	ACTION_FAILURE = 5,
	OFFBOARD_FAILURE = 6,
	UNKNOWN_ERROR = 255
};

// Struct to save a system and its telemetry and action objects
struct SystemPlugins {
	SystemPlugins(shared_ptr<System> system) {
		this->system = system;
		telemetry = std::make_shared<Telemetry>(system);
		action = std::make_shared<Action>(system);
		offboard = std::make_shared<Offboard>(system);
	}

	shared_ptr<System> system;
	shared_ptr<Telemetry> telemetry;
	shared_ptr<Action> action;
	shared_ptr<Offboard> offboard;
};

void establish_connections(int argc, char *argv[], Mavsdk &mavsdk);
void wait_systems(Mavsdk &mavsdk, const vector<System>::size_type expected_systems);

shared_ptr<Logger> logger{new ThreadStandardLogger};
shared_ptr<Level> info{new Info};
shared_ptr<Level> error{new Error};
shared_ptr<Level> debug{new Debug};

int main(int argc, char *argv[]) {
	if (argc < 2) {
		logger->write(error, "Use: " + string(argv[0]) + " <port1> <port2> ...");

		exit(static_cast<int>(ProRetCod::BAD_ARGUMENT));
	}

	// Constants
	const vector<System>::size_type expected_systems{static_cast<unsigned long>(argc - 1)};

	Mavsdk mavsdk;

	shared_ptr<Flag> flag{new RandomFlag{}};

	logger->write(info, "The flag is in:\n" + static_cast<string>(*flag));

	establish_connections(argc, argv, mavsdk);
	wait_systems(mavsdk, expected_systems);

	for (shared_ptr<System> s : mavsdk.systems()) {
		logger->write(info, "System: " + std::to_string(static_cast<int>(s->get_system_id())) + "\n" +
			"    Is connected: " + string((s->is_connected()) ? "true" : "false") + "\n" +
			"    Has autopilot: " + string((s->has_autopilot()) ? "true" : "false")
		);
	}

	vector<SystemPlugins> system_plugins_list{};

	for (shared_ptr<System> s : mavsdk.systems()) {
		system_plugins_list.push_back(SystemPlugins(s));
	}

	vector<shared_ptr<std::thread>> threads_for_waiting{};

	// Defining shared thread variables
	bool operation_ok{true};
	std::mutex mut;
	std::string operation_name{"set rate position & velocity"};
	std::string next_operation_name{};
	ProRetCod error_code = ProRetCod::UNKNOWN_ERROR;
	std::barrier sync_point(expected_systems, [&operation_ok, &operation_name, &error_code]() {
			if (operation_ok) {
				logger->write(info, "Synchronization point: " + operation_name);
			} else {
				logger->write(error, "Operation \"" + operation_name + "\" fails");
				exit(static_cast<int>(error_code));
			}
		});

	for (auto &sp : system_plugins_list) {
		threads_for_waiting.push_back(std::make_unique<std::thread>(std::thread{[sp, &operation_ok, &operation_name, &mut, &sync_point, &error_code]() {
			// Sets the position packet sending rate
			logger->write(info, "Setting rate in system " + std::to_string(static_cast<int>(sp.system->get_system_id())));

			Telemetry::Result telemetry_result{sp.telemetry->set_rate_position_velocity_ned(1.0)};
			if (telemetry_result == Telemetry::Result::Success) {
				logger->write(info, "Correctly set rate in system " + std::to_string(static_cast<int>(sp.system->get_system_id())));
			} else {
				std::ostringstream os;
				os << telemetry_result;
				logger->write(error, "Failure to set rate in system " + std::to_string(static_cast<int>(sp.system->get_system_id())) + ": " + os.str());
				mut.lock();
				operation_ok = false;
				error_code = ProRetCod::TELEMETRY_FAILURE;
				mut.unlock();
			}

			sync_point.arrive_and_wait();

			// Check the health of all systems
			mut.lock();
			operation_name = "check system health";
			mut.unlock();

			logger->write(info, "Checking system " + std::to_string(static_cast<int>(sp.system->get_system_id())));

			bool all_ok{sp.telemetry->health_all_ok()}; 
			if (all_ok) {
				logger->write(info, "All OK in system " + std::to_string(static_cast<int>(sp.system->get_system_id())));
			} else {
				logger->write(error, "Not all OK in system " + std::to_string(static_cast<int>(sp.system->get_system_id())));
				Telemetry::Health health{sp.telemetry->health()};

				logger->write(debug, "System " + std::to_string(static_cast<int>(sp.system->get_system_id())) + "\n" +
					"    is accelerometer calibration OK: " + string(health.is_accelerometer_calibration_ok ? "true" : "false") + "\n" +
					"    is armable: " + string(health.is_armable ? "true" : "false") + "\n" +
					"    is global position OK: " + string(health.is_global_position_ok ? "true" : "false") + "\n" +
					"    is gyrometer calibration OK: " + string(health.is_gyrometer_calibration_ok ? "true" : "false") + "\n" +
					"    is home position OK: " + string(health.is_home_position_ok ? "true" : "false") + "\n" +
					"    is local position OK: " + string(health.is_local_position_ok ? "true" : "false") + "\n" +
					"    is magnetometer calibration OK: " + string(health.is_magnetometer_calibration_ok ? "true" : "false")
				);

				mut.lock();
				operation_ok = false;
				error_code = ProRetCod::TELEMETRY_FAILURE;
				mut.unlock();
			}

			sync_point.arrive_and_wait();

			// Set takeoff altitude
			mut.lock();
			operation_name = "set takeoff altitude";
			mut.unlock();

			logger->write(info, "Setting takeoff altitude of system " + std::to_string(static_cast<int>(sp.system->get_system_id())));

			Action::Result action_result{sp.action->set_takeoff_altitude(takeoff_altitude)}; 
			if (action_result == Action::Result::Success) {
				logger->write(info, "Takeoff altitude set on system " + std::to_string(static_cast<int>(sp.system->get_system_id())));
			} else {
				std::ostringstream os;
				os << action_result;
				logger->write(error, "Error setting takeoff altitude on system " + std::to_string(static_cast<int>(sp.system->get_system_id())) + ": " + os.str());

				mut.lock();
				operation_ok = false;
				error_code = ProRetCod::ACTION_FAILURE;
				mut.unlock();
			}

			sync_point.arrive_and_wait();

			// Arming systems
			mut.lock();
			operation_name = "arm systems";
			mut.unlock();

			logger->write(info, "Arming system " + std::to_string(static_cast<int>(sp.system->get_system_id())));

			action_result = sp.action->arm(); 
			if (action_result == Action::Result::Success) {
				logger->write(info, "System " + std::to_string(static_cast<int>(sp.system->get_system_id())) + " armed");
			} else {
				std::ostringstream os;
				os << action_result;
				logger->write(error, "Error arming system " + std::to_string(static_cast<int>(sp.system->get_system_id())) + ": " + os.str());

				mut.lock();
				operation_ok = false;
				error_code = ProRetCod::ACTION_FAILURE;
				mut.unlock();
			}

			sync_point.arrive_and_wait();

			// Takeoff
			mut.lock();
			operation_name = "take off";
			mut.unlock();

			logger->write(info, "Taking off system " + std::to_string(static_cast<int>(sp.system->get_system_id())));

			action_result = sp.action->takeoff(); 
			if (action_result == Action::Result::Success) {
				logger->write(info, "System " + std::to_string(static_cast<int>(sp.system->get_system_id())) + " taking off");

				// Waiting to finish takeoff
				float current_altitude{sp.telemetry->position().relative_altitude_m};
				logger->write(debug, "System " + std::to_string(static_cast<int>(sp.system->get_system_id())) + " altitude: " + std::to_string(current_altitude));
				while ((std::isnan(current_altitude)) or (current_altitude < takeoff_altitude - reasonable_error)) {
					std::this_thread::sleep_for(std::chrono::seconds{refresh_time});
					current_altitude = sp.telemetry->position().relative_altitude_m;
					logger->write(debug, "System " + std::to_string(static_cast<int>(sp.system->get_system_id())) + " altitude: " + std::to_string(current_altitude));
				}
			} else {
				std::ostringstream os;
				os << action_result;
				logger->write(error, "Error taking off system " + std::to_string(static_cast<int>(sp.system->get_system_id())) + ": " + os.str());

				mut.lock();
				operation_ok = false;
				error_code = ProRetCod::ACTION_FAILURE;
				mut.unlock();
			}

			sync_point.arrive_and_wait();

			// Setting a setpoint
			mut.lock();
			operation_name = "set setpoint";
			mut.unlock();

			logger->write(info, "Setting setpoint on system " + std::to_string(static_cast<int>(sp.system->get_system_id())));

			Offboard::PositionNedYaw expected_position_yaw{10.0f, 0.0f, -10.0f, 0.0f};
			Offboard::Result offboard_result{sp.offboard->set_position_ned(expected_position_yaw)}; 
			if (offboard_result == Offboard::Result::Success) {
				logger->write(info, "Setpoint of system " + std::to_string(static_cast<int>(sp.system->get_system_id())) + " set");
			} else {
				std::ostringstream os;
				os << offboard_result;
				logger->write(error, "Error setting setpoint on system " + std::to_string(static_cast<int>(sp.system->get_system_id())) + ": " + os.str());

				mut.lock();
				operation_ok = false;
				error_code = ProRetCod::OFFBOARD_FAILURE;
				mut.unlock();
			}

			sync_point.arrive_and_wait();

			// Start offboard mode
			mut.lock();
			operation_name = "start offboard mode";
			mut.unlock();

			Telemetry::PositionNed expected_position{expected_position_yaw.north_m, expected_position_yaw.east_m, expected_position_yaw.down_m};
			std::mutex waiting_mutex;
			waiting_mutex.lock();

			logger->write(info, "Starting offboard mode on system " + std::to_string(static_cast<int>(sp.system->get_system_id())));

			offboard_result = sp.offboard->start();
			if (offboard_result == Offboard::Result::Success) {
				logger->write(info, "System " + std::to_string(static_cast<int>(sp.system->get_system_id())) + " in offboard mode");

				Telemetry::PositionVelocityNedHandle handle {sp.telemetry->subscribe_position_velocity_ned([&waiting_mutex, &expected_position, &handle, &sp](Telemetry::PositionVelocityNed pos) {
					logger->write(debug, "System " + std::to_string(static_cast<int>(sp.system->get_system_id())) + " position & velocity:\n" +
						"Pos North: " + std::to_string(pos.position.north_m) + "\n" +
						"Pos East: " + std::to_string(pos.position.east_m) + "\n" +
						"Pos Down: " + std::to_string(pos.position.down_m) + "\n" +
						"Vel North: " + std::to_string(pos.velocity.north_m_s) + "\n" +
						"Vel East: " + std::to_string(pos.velocity.east_m_s) + "\n" +
						"Vel Down: " + std::to_string(pos.velocity.down_m_s) + "\n"
					);

					if ((pos.position.down_m <= expected_position.down_m + reasonable_error) and
						(pos.position.down_m >= expected_position.down_m - reasonable_error) and
						(pos.position.east_m <= expected_position.east_m + reasonable_error) and
						(pos.position.east_m >= expected_position.east_m - reasonable_error) and
						(pos.position.north_m <= expected_position.north_m + reasonable_error) and
						(pos.position.north_m >= expected_position.north_m - reasonable_error)
					) {
						sp.telemetry->unsubscribe_position_velocity_ned(handle);
						logger->write(info, "System " + std::to_string(static_cast<int>(sp.system->get_system_id())) + " has reached the target");
						waiting_mutex.unlock();
					}
				})};

				waiting_mutex.lock();

			} else {
				std::ostringstream os;
				os << offboard_result;
				logger->write(error, "Error starting offboard mode on system " + std::to_string(static_cast<int>(sp.system->get_system_id())) + ": " + os.str());

				mut.lock();
				operation_ok = false;
				error_code = ProRetCod::OFFBOARD_FAILURE;
				mut.unlock();
			}

			sync_point.arrive_and_wait();

			// Hold position
			mut.lock();
			operation_name = "hold position";
			mut.unlock();

			action_result = sp.action->hold();
			if (action_result == Action::Result::Success) {
				logger->write(info, "System " + std::to_string(static_cast<int>(sp.system->get_system_id())) + " holding position");
			} else {
				std::ostringstream os;
				os << action_result;
				logger->write(error, "Error holding position on system " + std::to_string(static_cast<int>(sp.system->get_system_id())) + ": " + os.str());

				mut.lock();
				operation_ok = false;
				error_code = ProRetCod::ACTION_FAILURE;
				mut.unlock();
			}
		}}));
	}

	for (shared_ptr<std::thread> th : threads_for_waiting) {
		th->join();
	}

/*
	// Landing
//	threads_for_waiting.clear();
//	operation_ok = true;
//	sp = system_plugins_list.begin();
//	while ((check_operation_ok(operation_ok, mut)) and (sp != system_plugins_list.end())) {
//		threads_for_waiting.push_back(std::make_unique<std::thread>(std::thread{[sp, &operation_ok, &mut]() {
//			cout << "Landing system " << static_cast<int>(sp->system->get_system_id()) << endl;
//
//			Action::Result result{sp->action->land()}; 
//			if (result == Action::Result::Success) {
//				cout << "System " << static_cast<int>(sp->system->get_system_id()) << " landing"<< endl;
//
//				// Waiting to finish landing
//				while (sp->telemetry->armed()) {
//					cout << "System " << static_cast<int>(sp->system->get_system_id()) << " landing"<< endl;
//					std::this_thread::sleep_for(std::chrono::seconds{refresh_time});
//				}
//			} else {
//				cerr << "Error landing system " << static_cast<int>(sp->system->get_system_id()) << endl;
//				cerr << result << endl;
//
//				mut.lock();
//				operation_ok = false;
//				mut.unlock();
//			}
//		}}));
//
//		std::advance(sp, 1);
//	}
//
//	for (shared_ptr<std::thread> th : threads_for_waiting) {
//		th->join();
//	}
//
//    if (operation_ok) {
//		cout << "All systems on ground" << endl;
//	} else {
//		exit(static_cast<int>(ProRetCod::ACTION_FAILURE));
//	}
	*/

	return static_cast<int>(ProRetCod::OK);
}

void establish_connections(int argc, char *argv[], Mavsdk &mavsdk) {
	// Store the URLs to access the drones
	vector<std::string> url_list{};

	for (int i {1}; i < argc; ++i) {
		url_list.push_back("udp://:" + std::string {argv[i]});
	}

	logger->write(info, "Establishing connection...");
	for (std::string url : url_list) {
		ConnectionResult connection_result{mavsdk.add_any_connection(url)};
		if (connection_result == ConnectionResult::Success) {
			logger->write(info, "Connection established on " + url);
		} else {
			std::ostringstream os;
			os << connection_result;
			logger->write(error, "Connection failed on " + url + ": " + os.str());

			exit(static_cast<int>(ProRetCod::CONNECTION_FAILED));
		}
	}
}

void wait_systems(Mavsdk &mavsdk, const vector<System>::size_type expected_systems) {
	vector<System>::size_type discovered_systems {mavsdk.systems().size()};

	std::promise<void> prom{};
	std::future fut{prom.get_future()};
	unsigned int times_executed {0};

	Mavsdk::NewSystemHandle system_handle = mavsdk.subscribe_on_new_system([&mavsdk, &discovered_systems, expected_systems, &prom, &times_executed, &system_handle]() {
		discovered_systems = mavsdk.systems().size();
		vector<System>::size_type remaining_systems {expected_systems - discovered_systems};

		shared_ptr<System> s {mavsdk.systems().back()};
		logger->write(info, "Systems found: " + std::to_string(discovered_systems));
		logger->write(info, "Remaining systems: " + std::to_string(remaining_systems));

		++times_executed;

		if (times_executed == expected_systems) {
			mavsdk.unsubscribe_on_new_system(system_handle);
			prom.set_value();
		}
	});
	
	if (fut.wait_for(max_waiting_time) != std::future_status::ready) {
		logger->write(error, "Not all systems found");
		exit(static_cast<int>(ProRetCod::NO_SYSTEMS_FOUND));
	} else {
		logger->write(info, "Systems search completed");
	}
}