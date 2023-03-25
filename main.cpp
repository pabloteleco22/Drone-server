#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/offboard/offboard.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include "lib/flag/flag.hpp"
#include <string>
#include "lib/log/log.hpp"

using namespace mavsdk;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::shared_ptr;

// Constants
const std::chrono::seconds max_waiting_time{30};
const float takeoff_altitude{3.0F};
const int64_t refresh_time{1L};

// Process return code
enum class ProRetCod : int {
	OK = 0,
	BAD_ARGUMENT = 1,
	CONNECTION_FAILED = 2,
	NO_SYSTEMS_FOUND = 3,
	TELEMETRY_FAILURE = 4,
	ACTION_FAILURE = 5,
	OFFBOARD_FAILURE = 6
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
bool check_operation_ok(bool &operation_ok, std::mutex &mut);

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cerr << "Use: " << "<port1> <port2> ..." << endl;

		exit(static_cast<int>(ProRetCod::BAD_ARGUMENT));
	}

	// Constants
	const vector<System>::size_type expected_systems{static_cast<unsigned long>(argc - 1)};

	Mavsdk mavsdk;
	std::mutex mut;

	shared_ptr<Flag> flag{new RandomFlag{}};

	cout << "The flag is in:" << endl << (std::string)(*flag) << endl;

	establish_connections(argc, argv, mavsdk);
	wait_systems(mavsdk, expected_systems);

	for (shared_ptr<System> s : mavsdk.systems()) {
		cout << "System: " << static_cast<int>(s->get_system_id()) << endl;
		cout << "    Is connected: " << std::boolalpha << s->is_connected() << endl;
		cout << "    Has autopilot: " << std::boolalpha << s->has_autopilot() << endl;
	}

	vector<SystemPlugins> system_plugins_list{};

	for (shared_ptr<System> s : mavsdk.systems()) {
		system_plugins_list.push_back(SystemPlugins(s));
	}

	vector<shared_ptr<std::thread>> threads_for_waiting{};


	// Sets the position packet sending rate
	bool operation_ok{true};
	vector<SystemPlugins>::iterator sp{system_plugins_list.begin()};
	while ((check_operation_ok(operation_ok, mut)) and (sp != system_plugins_list.end())) {
		threads_for_waiting.push_back(std::make_unique<std::thread>(std::thread{[sp, &operation_ok, &mut]() {
			cout << "Setting rate in system " << static_cast<int>(sp->system->get_system_id()) << endl;

			Telemetry::Result result{sp->telemetry->set_rate_position_velocity_ned(1.0)};
			if (result == Telemetry::Result::Success) {
				cout << "Correctly set rate in system " << static_cast<int>(sp->system->get_system_id()) << endl;
			} else {
				cerr << "Failure to set rate in system " << static_cast<int>(sp->system->get_system_id()) << endl;
				mut.lock();
				operation_ok = false;
				mut.unlock();
			}
		}}));

		std::advance(sp, 1);
	}

	for (shared_ptr<std::thread> th : threads_for_waiting) {
		th->join();
	}

    if (operation_ok) {
		cout << "All rates defined" << endl;
	} else {
		exit(static_cast<int>(ProRetCod::TELEMETRY_FAILURE));
	}

	// Check the health of all systems
	threads_for_waiting.clear();
	operation_ok = true;
	sp = system_plugins_list.begin();

	// TODO: Implements threads with barriers
	while ((check_operation_ok(operation_ok, mut)) and (sp != system_plugins_list.end())) {
		threads_for_waiting.push_back(std::make_unique<std::thread>(std::thread{[sp, &operation_ok, &mut]() {
			mut.lock();
			cout << "Checking system " << static_cast<int>(sp->system->get_system_id()) << endl;
			mut.unlock();

			bool all_ok{sp->telemetry->health_all_ok()}; 
			if (all_ok) {
				mut.lock();
				cout << "All ok in system " << static_cast<int>(sp->system->get_system_id()) << endl;
				mut.unlock();
			} else {
				mut.lock();
				cerr << "Not all ok in system " << static_cast<int>(sp->system->get_system_id()) << endl;
				mut.unlock();
				Telemetry::Health health{sp->telemetry->health()};

				mut.lock();
				cerr << "System " << static_cast<int>(sp->system->get_system_id()) << endl;
				cout << "is accelerometer calibration ok: " << std::boolalpha << health.is_accelerometer_calibration_ok << endl;
				cout << "is armable: " << std::boolalpha << health.is_armable << endl;
				cout << "is global position ok: " << std::boolalpha << health.is_global_position_ok << endl;
				cout << "is gyrometer calibration ok: " << std::boolalpha << health.is_gyrometer_calibration_ok << endl;
				cout << "is home position ok: " << std::boolalpha << health.is_home_position_ok << endl;
				cout << "is local position ok: " << std::boolalpha << health.is_local_position_ok << endl;
				cout << "is magnetometer calibration ok: " << std::boolalpha << health.is_magnetometer_calibration_ok << endl;

				operation_ok = false;
				mut.unlock();
			}
		}}));

		std::advance(sp, 1);
	}

	for (shared_ptr<std::thread> th : threads_for_waiting) {
		th->join();
	}

    if (operation_ok) {
		cout << "All systems ok" << endl;
	} else {
		exit(static_cast<int>(ProRetCod::TELEMETRY_FAILURE));
	}

	// Set takeoff altitude
	threads_for_waiting.clear();
	operation_ok = true;
	sp = system_plugins_list.begin();
	while ((check_operation_ok(operation_ok, mut)) and (sp != system_plugins_list.end())) {
		threads_for_waiting.push_back(std::make_unique<std::thread>(std::thread{[sp, &operation_ok, &mut]() {
			cout << "Setting takeoff altitude of system " << static_cast<int>(sp->system->get_system_id()) << endl;

			Action::Result result{sp->action->set_takeoff_altitude(takeoff_altitude)}; 
			if (result == Action::Result::Success) {
				cout << "Takeoff altitude set on system " << static_cast<int>(sp->system->get_system_id()) << endl;
			} else {
				cerr << "Error setting takeoff altitude on system " << static_cast<int>(sp->system->get_system_id()) << endl;
				cerr << result << endl;

				mut.lock();
				operation_ok = false;
				mut.unlock();
			}
		}}));

		std::advance(sp, 1);
	}

	for (shared_ptr<std::thread> th : threads_for_waiting) {
		th->join();
	}

    if (operation_ok) {
		cout << "Takeoff altitude of all systems set" << endl;
	} else {
		exit(static_cast<int>(ProRetCod::ACTION_FAILURE));
	}

	// Arming systems
	threads_for_waiting.clear();
	operation_ok = true;
	sp = system_plugins_list.begin();
	while ((check_operation_ok(operation_ok, mut)) and (sp != system_plugins_list.end())) {
		threads_for_waiting.push_back(std::make_unique<std::thread>(std::thread{[sp, &operation_ok, &mut]() {
			cout << "Arming system " << static_cast<int>(sp->system->get_system_id()) << endl;

			Action::Result result{sp->action->arm()}; 
			if (result == Action::Result::Success) {
				cout << "System armed " << static_cast<int>(sp->system->get_system_id()) << endl;
			} else {
				cerr << "Error arming system " << static_cast<int>(sp->system->get_system_id()) << endl;
				cerr << result << endl;

				mut.lock();
				operation_ok = false;
				mut.unlock();
			}
		}}));

		std::advance(sp, 1);
	}

	for (shared_ptr<std::thread> th : threads_for_waiting) {
		th->join();
	}

    if (operation_ok) {
		cout << "All armed" << endl;
	} else {
		exit(static_cast<int>(ProRetCod::ACTION_FAILURE));
	}

	// Taking off
	threads_for_waiting.clear();
	operation_ok = true;
	sp = system_plugins_list.begin();
	while ((check_operation_ok(operation_ok, mut)) and (sp != system_plugins_list.end())) {
		threads_for_waiting.push_back(std::make_unique<std::thread>(std::thread{[sp, &operation_ok, &mut]() {
			cout << "Taking off system " << static_cast<int>(sp->system->get_system_id()) << endl;

			Action::Result result{sp->action->takeoff()}; 
			if (result == Action::Result::Success) {
				cout << "System " << static_cast<int>(sp->system->get_system_id()) << " taking off" << endl;

				// Waiting to finish takeoff
				float current_altitude{sp->telemetry->position().relative_altitude_m};
				cout << "Altitud sistema " << static_cast<int>(sp->system->get_system_id()) << ": " << current_altitude << endl;
				while ((std::isnan(current_altitude)) or (current_altitude < takeoff_altitude)) {
					cout << "System " << static_cast<int>(sp->system->get_system_id()) << " taking off" << endl;
					std::this_thread::sleep_for(std::chrono::seconds{refresh_time});
					current_altitude = sp->telemetry->position().relative_altitude_m;
				}
			} else {
				cerr << "Error taking off system " << static_cast<int>(sp->system->get_system_id()) << endl;
				cerr << result << endl;

				mut.lock();
				operation_ok = false;
				mut.unlock();
			}
		}}));

		std::advance(sp, 1);
	}

	for (shared_ptr<std::thread> th : threads_for_waiting) {
		th->join();
	}

    if (operation_ok) {
		cout << "All systems on air" << endl;
	} else {
		exit(static_cast<int>(ProRetCod::ACTION_FAILURE));
	}

	// Setting a setpoint
	threads_for_waiting.clear();
	operation_ok = true;
	sp = system_plugins_list.begin();
	Offboard::PositionNedYaw expected_position_yaw{10.0f, 0.0f, -10.0f, 0.0f};
	while ((check_operation_ok(operation_ok, mut)) and (sp != system_plugins_list.end())) {
		threads_for_waiting.push_back(std::make_unique<std::thread>(std::thread{[sp, &operation_ok, &mut, &expected_position_yaw]() {
			cout << "System " << static_cast<int>(sp->system->get_system_id()) << " go 10 meters forward" << endl;

			Offboard::Result result{sp->offboard->set_position_ned(expected_position_yaw)}; 
			if (result == Offboard::Result::Success) {
				cout << "System " << static_cast<int>(sp->system->get_system_id()) << " moving"<< endl;
			} else {
				cerr << "Error moving system " << static_cast<int>(sp->system->get_system_id()) << endl;
				cerr << result << endl;

				mut.lock();
				operation_ok = false;
				mut.unlock();
			}
		}}));

		std::advance(sp, 1);
	}

	for (shared_ptr<std::thread> th : threads_for_waiting) {
		th->join();
	}

    if (operation_ok) {
		cout << "All systems on offboard mode" << endl;
	} else {
		exit(static_cast<int>(ProRetCod::OFFBOARD_FAILURE));
	}

	// Start offboard mode
	threads_for_waiting.clear();
	operation_ok = true;
	sp = system_plugins_list.begin();
	while ((check_operation_ok(operation_ok, mut)) and (sp != system_plugins_list.end())) {
		threads_for_waiting.push_back(std::make_unique<std::thread>(std::thread{[sp, &operation_ok, &mut, &expected_position_yaw]() {
			Telemetry::PositionNed expected_position{expected_position_yaw.north_m, expected_position_yaw.east_m, expected_position_yaw.down_m};
			std::mutex waiting_mutex;
			waiting_mutex.lock();

			cout << "Starting offboard mode on system " << static_cast<int>(sp->system->get_system_id()) << endl;

			Offboard::Result result{sp->offboard->start()}; 
			if (result == Offboard::Result::Success) {
				cout << "System " << static_cast<int>(sp->system->get_system_id()) << " in offboard mode"<< endl;

				Telemetry::PositionVelocityNedHandle handle {sp->telemetry->subscribe_position_velocity_ned([&waiting_mutex, &expected_position, &handle, &sp](Telemetry::PositionVelocityNed pos) {
					cout << "Pos North: " << pos.position.north_m << endl;
					cout << "Pos East: " << pos.position.east_m << endl;
					cout << "Pos Down: " << pos.position.down_m << endl;
					cout << "Vel North: " << pos.velocity.north_m_s << endl;
					cout << "Vel East: " << pos.velocity.east_m_s << endl;
					cout << "Vel Down: " << pos.velocity.down_m_s << endl;

					if ((pos.position.down_m != expected_position.down_m) or
						(pos.position.east_m != expected_position.east_m) or
						(pos.position.north_m != expected_position.north_m)
					) {
						sp->telemetry->unsubscribe_position_velocity_ned(handle);
						//waiting_mutex.unlock();
						cout << "Se ha alcanzado el objetivo" << endl;
					}
				})};

				waiting_mutex.lock();

			} else {
				cerr << "Error starting offboard mode on system " << static_cast<int>(sp->system->get_system_id()) << endl;
				cerr << result << endl;

				mut.lock();
				operation_ok = false;
				mut.unlock();
			}
		}}));

		std::advance(sp, 1);
	}

	for (shared_ptr<std::thread> th : threads_for_waiting) {
		th->join();
	}

    if (operation_ok) {
		cout << "All systems on offboard mode" << endl;
	} else {
		exit(static_cast<int>(ProRetCod::OFFBOARD_FAILURE));
	}

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

	return static_cast<int>(ProRetCod::OK);
}

void establish_connections(int argc, char *argv[], Mavsdk &mavsdk) {
	// Store the URLs to access the drones
	vector<std::string> url_list{};

	for (int i {1}; i < argc; ++i) {
		url_list.push_back("udp://:" + std::string {argv[i]});
	}

	cout << "Establishing connection..." << endl;
	for (std::string url : url_list) {
		ConnectionResult connection_result{mavsdk.add_any_connection(url)};
		if (connection_result == ConnectionResult::Success) {
			cout << "Connection established on " << url << endl;
		} else {
			cerr << "Connection failed on " << url << ": " << connection_result << endl;

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
		cout << "Systems found: " << discovered_systems << endl;
		cout << "Remaining systems: " << remaining_systems << endl;

		++times_executed;

		if (times_executed == expected_systems) {
			mavsdk.unsubscribe_on_new_system(system_handle);
			prom.set_value();
		}
	});
	
	if (fut.wait_for(max_waiting_time) != std::future_status::ready) {
		cout << "Not all systems found" << endl;
		exit(static_cast<int>(ProRetCod::NO_SYSTEMS_FOUND));
	} else {
		cout << "Systems search completed" << endl;
	}
}

bool check_operation_ok(bool &operation_ok, std::mutex &mut) {
	bool ret;

	mut.lock();
	ret = operation_ok;
	mut.unlock();

	return ret;
}