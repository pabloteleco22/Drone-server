#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/offboard/offboard.h>
#include <mavsdk/log_callback.h>
#include "lib/flag/flag.hpp"
#include "lib/log/log.hpp"
#include "lib/poly/polygon.hpp"
#include "lib/missionhelper/missionhelper.hpp"
#include <thread>
#include <barrier>
#include <chrono>
#include <future>
#include <string>
#include <sstream>
#include <fstream>

#define UNUSED(x) (void)(x)

using namespace mavsdk;
using std::vector;
using std::shared_ptr;

// Constants
const std::chrono::seconds max_waiting_time{10};
const float takeoff_altitude{3.0F};
const std::chrono::seconds refresh_time{1};
const float reasonable_error{0.3};
const float percentage_drones_required{66};
const unsigned int max_attempts{10};

// Process return code
enum class ProRetCod : int {
	OK = 0,
	BAD_ARGUMENT = 1,
	CONNECTION_FAILED = 2,
	NO_SYSTEMS_FOUND = 3,
	TELEMETRY_FAILURE = 4,
	ACTION_FAILURE = 5,
	OFFBOARD_FAILURE = 6,
	MISSION_FAILURE = 7,
	UNKNOWN_ERROR = 255
};

// Struct to save a system and its telemetry and action objects
struct SystemPlugins {
	SystemPlugins(shared_ptr<System> system) {
		this->system = system;
		telemetry = std::make_shared<Telemetry>(system);
		action = std::make_shared<Action>(system);
		offboard = std::make_shared<Offboard>(system);
		mission = std::make_shared<Mission>(system);
	}

	shared_ptr<System> system;
	shared_ptr<Telemetry> telemetry;
	shared_ptr<Action> action;
	shared_ptr<Offboard> offboard;
	shared_ptr<Mission> mission;
};

struct CheckEnoughSystems {
	virtual bool exists_enough_systems(const float number_of_systems) = 0;
	virtual string get_status(const float number_of_systems) = 0;
};

struct PercentageCheck final : public CheckEnoughSystems {
	PercentageCheck(const float expected_systems, const float percentage_drones_required=100.0f) {
		this->expected_systems = expected_systems;
		this->percentage_required = percentage_drones_required;
		required_systems = expected_systems * percentage_drones_required / 100.0f;
	}

	bool exists_enough_systems(const float number_of_systems) override {
		return number_of_systems >= required_systems;
	}

	string get_status(const float number_of_systems) override {
		return "Required percentage " + std::to_string(percentage_required) + "%. Systems in use " +
															std::to_string(100.0f * number_of_systems / expected_systems) + "%";
	}

	private:
		float percentage_required;
		float expected_systems;
		float required_systems;
};

void establish_connections(int argc, char *argv[], Mavsdk &mavsdk);
float wait_systems(Mavsdk &mavsdk, const vector<System>::size_type expected_systems, shared_ptr<CheckEnoughSystems> enough_systems);

shared_ptr<LoggerDecoration> logger_decoration{new HourLoggerDecoration};
shared_ptr<std::ostream> stream{new std::ofstream{"logs/last_execution.log"}};
shared_ptr<Logger> logger{new BiLogger{
	new ThreadLogger{new StandardLogger{logger_decoration}},
	new ThreadLogger{new StreamLogger{stream, logger_decoration}}
}};
shared_ptr<Level> error{new Error};
shared_ptr<Level> warning{new Warning};
shared_ptr<Level> info{new Info};
shared_ptr<Level> debug{new Debug};

int main(int argc, char *argv[]) {
	if (argc < 2) {
		logger->write(error, "Use: " + string(argv[0]) + " <port1> <port2> ...");

		exit(static_cast<int>(ProRetCod::BAD_ARGUMENT));
	}

	// Constants
	const vector<System>::size_type expected_systems{static_cast<unsigned long>(argc - 1)};

	log::subscribe([](log::Level level,   // message severity level
                          const std::string& message, // message text
                          const std::string& file,    // source file from which the message was sent
                          int line) {                 // line number in the source file
		UNUSED(message);
		UNUSED(file);
		UNUSED(line);
		UNUSED(level);
		// returning true from the callback disables printing the message to stdout
		//return level < log::Level::Warn;
		return true;
	});

	Mavsdk mavsdk;


	shared_ptr<Flag> flag{new RandomFlag{}};

	shared_ptr<Polygon> search_area{new Polygon};
	search_area->push_back({static_cast<double>(RandomFlag::default_east_m.get_min()), static_cast<double>(RandomFlag::default_north_m.get_min())});
	search_area->push_back({static_cast<double>(RandomFlag::default_east_m.get_min()), static_cast<double>(RandomFlag::default_north_m.get_max())});
	search_area->push_back({static_cast<double>(RandomFlag::default_east_m.get_max()), static_cast<double>(RandomFlag::default_north_m.get_max())});
	search_area->push_back({static_cast<double>(RandomFlag::default_east_m.get_max()), static_cast<double>(RandomFlag::default_north_m.get_min())});

	//search_area->push_back({0, 0});
	//search_area->push_back({0, 10});
	//search_area->push_back({10, 10});
	//search_area->push_back({10, 0});

	shared_ptr<MissionHelper> mission_helper{new ParallelSweep{search_area}};

	logger->write(info, "The flag is in:\n" + static_cast<string>(*flag));

	shared_ptr<CheckEnoughSystems> enough_systems{new PercentageCheck{static_cast<float>(expected_systems), percentage_drones_required}};

	establish_connections(argc, argv, mavsdk);
	float final_systems{wait_systems(mavsdk, expected_systems, enough_systems)};

	vector<SystemPlugins> system_plugins_list{};

	for (shared_ptr<System> s : mavsdk.systems()) {
		logger->write(debug, "System: " + std::to_string(static_cast<int>(s->get_system_id())) + "\n" +
			"    Is connected: " + string((s->is_connected()) ? "true" : "false") + "\n" +
			"    Has autopilot: " + string((s->has_autopilot()) ? "true" : "false") + "\n"
		);

		system_plugins_list.push_back(SystemPlugins(s));
	}

	vector<shared_ptr<std::thread>> threads_for_waiting{};

	// Defining shared thread variables
	bool operation_ok{true};
	std::mutex mut;
	std::string operation_name{"set rate position & velocity"};
	std::string next_operation_name{};
	ProRetCod error_code = ProRetCod::UNKNOWN_ERROR;
	std::barrier sync_point{static_cast<std::ptrdiff_t>(final_systems), [&operation_ok, &operation_name, &error_code]() {
			if (operation_ok) {
				logger->write(info, "Synchronization point: " + operation_name);
			} else {
				logger->write(error, "Operation \"" + operation_name + "\" fails");
				exit(static_cast<int>(error_code));
			}
		}};

	for (auto &sp : system_plugins_list) {
		threads_for_waiting.push_back(
			std::make_unique<std::thread>(std::thread{
				[sp, &operation_ok, &operation_name, &mut, &sync_point, &error_code, &final_systems, &mission_helper, &enough_systems]() {
			unsigned int system_id{static_cast<unsigned int>(sp.system->get_system_id())};

			// Sets the position packet sending rate
			logger->write(info, "Setting rate in system " + std::to_string(system_id));

			Telemetry::Result telemetry_result{sp.telemetry->set_rate_position_velocity_ned(1.0)};
			if (telemetry_result == Telemetry::Result::Success) {
				logger->write(info, "Correctly set rate in system " + std::to_string(system_id));
			} else {
				std::ostringstream os;
				os << telemetry_result;
				logger->write(error, "Failure to set rate in system " + std::to_string(system_id) + ": " + os.str());
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

			logger->write(info, "Checking system " + std::to_string(system_id));

			bool all_ok{sp.telemetry->health_all_ok()}; 
			unsigned int attempts{max_attempts};
			while ((not all_ok) and (attempts > 0)) {
				--attempts;

				logger->write(error, "Not all OK in system " + std::to_string(system_id) + ". Remaining attempts: " + std::to_string(attempts));
				Telemetry::Health health{sp.telemetry->health()};

				logger->write(info, "System " + std::to_string(system_id) + "\n" +
					"    is accelerometer calibration OK: " + string(health.is_accelerometer_calibration_ok ? "true" : "false") + "\n" +
					"    is armable: " + string(health.is_armable ? "true" : "false") + "\n" +
					"    is global position OK: " + string(health.is_global_position_ok ? "true" : "false") + "\n" +
					"    is gyrometer calibration OK: " + string(health.is_gyrometer_calibration_ok ? "true" : "false") + "\n" +
					"    is home position OK: " + string(health.is_home_position_ok ? "true" : "false") + "\n" +
					"    is local position OK: " + string(health.is_local_position_ok ? "true" : "false") + "\n" +
					"    is magnetometer calibration OK: " + string(health.is_magnetometer_calibration_ok ? "true" : "false")
				);

				std::this_thread::sleep_for(refresh_time);

				all_ok = sp.telemetry->health_all_ok(); 
			}

			if (all_ok) {
				logger->write(info, "All OK in system " + std::to_string(system_id));
				sync_point.arrive_and_wait();
			} else {
				mut.lock();
				--final_systems;
				if (not enough_systems->exists_enough_systems(final_systems)) {
					operation_ok = false;
					error_code = ProRetCod::TELEMETRY_FAILURE;
				}
				mut.unlock();
				logger->write(warning, "System " + std::to_string(system_id) + " discarded. " + enough_systems->get_status(final_systems));
				sync_point.arrive_and_drop();
				return;
			}

			// Arming systems
			mut.lock();
			operation_name = "arm systems";
			mut.unlock();

			logger->write(info, "Arming system " + std::to_string(system_id));

			Action::Result action_result{sp.action->arm()}; 
			attempts = max_attempts;
			while ((action_result != Action::Result::Success) and (attempts > 0)) {
				--attempts;

				std::ostringstream os;
				os << action_result;
				logger->write(error, "Error arming system " + std::to_string(system_id) + ": " + os.str());

				std::this_thread::sleep_for(refresh_time);

				action_result = sp.action->arm();
			}

			if (action_result == Action::Result::Success) {
				logger->write(info, "System " + std::to_string(system_id) + " armed");
				sync_point.arrive_and_wait();
			} else {
				mut.lock();
				--final_systems;
				if (not enough_systems->exists_enough_systems(final_systems)) {
					operation_ok = false;
					error_code = ProRetCod::ACTION_FAILURE;
				}
				mut.unlock();
				logger->write(warning, "System " + std::to_string(system_id) + " discarded. " + enough_systems->get_status(final_systems));
				sync_point.arrive_and_drop();
				return;
			}

			// Set mission plan
			mut.lock();
			operation_name = "set mission plan";
			mut.unlock();

			logger->write(info, "Uploading mission plan to system " + std::to_string(system_id));

			vector<Mission::MissionItem> mission_item_vector;

			try {
				mission_helper->new_mission(system_id, final_systems, mission_item_vector);
			} catch (const CannotMakeMission &e) {
				if (std::string(e.what()) == "CannotMakeMission: The system ID must be less than or equal to the number of systems")
					mission_helper->new_mission(final_systems, final_systems, mission_item_vector);
			}

			for (auto p : mission_item_vector) {
				logger->write(debug, "System " + std::to_string(system_id) + " mission.\n    Latitude: " + std::to_string(p.latitude_deg) +
				"\n    Longitude: " + std::to_string(p.longitude_deg));
			}

			Mission::MissionPlan mission_plan;
			mission_plan.mission_items = mission_item_vector;

			Mission::Result mission_result{sp.mission->upload_mission(mission_plan)};

			while ((mission_result != Mission::Result::Success) and (attempts > 0)) {
				--attempts;

				std::ostringstream os;
				os << mission_result;
				logger->write(error, "Error uploading mission plan to system " + std::to_string(system_id) + ": " + os.str() + ". Remaining attempts: " + std::to_string(attempts));

				std::this_thread::sleep_for(refresh_time);

				mission_result = sp.mission->upload_mission(mission_plan);
			}

			if (mission_result == Mission::Result::Success) {
				logger->write(info, "Mission plan uploaded to system " + std::to_string(system_id));
			} else {
				std::ostringstream os;
				os << mission_result;
				logger->write(error, "Error uploading mission plan to system " + std::to_string(system_id) + ": " + os.str());

				mut.lock();
				operation_ok = false;
				error_code = ProRetCod::MISSION_FAILURE;
				mut.unlock();
			}

			sync_point.arrive_and_wait();

			// Start mission
			mut.lock();
			operation_name = "start mission";
			mut.unlock();

			mission_result = sp.mission->start_mission();

			if (mission_result == Mission::Result::Success) {
				logger->write(info, "The mission has started correctly in system " + std::to_string(system_id));
			} else {
				std::ostringstream os;
				os << mission_result;
				logger->write(error, "Error starting mission on system " + std::to_string(system_id) + ": " + os.str());

				mut.lock();
				operation_ok = false;
				error_code = ProRetCod::MISSION_FAILURE;
				mut.unlock();
			}

			/*
			// Set takeoff altitude
			mut.lock();
			operation_name = "set takeoff altitude";
			mut.unlock();

			logger->write(info, "Setting takeoff altitude of system " + std::to_string(system_id));

			Action::Result action_result{sp.action->set_takeoff_altitude(takeoff_altitude)}; 
			if (action_result == Action::Result::Success) {
				logger->write(info, "Takeoff altitude set on system " + std::to_string(system_id));
			} else {
				std::ostringstream os;
				os << action_result;
				logger->write(error, "Error setting takeoff altitude on system " + std::to_string(system_id) + ": " + os.str());

				mut.lock();
				operation_ok = false;
				error_code = ProRetCod::ACTION_FAILURE;
				mut.unlock();
			}

			sync_point.arrive_and_wait();
			*/

			/*
			// Arming systems
			mut.lock();
			operation_name = "arm systems";
			mut.unlock();

			logger->write(info, "Arming system " + std::to_string(system_id));

			action_result = sp.action->arm(); 
			attempts = max_attempts;
			while ((action_result != Action::Result::Success) and (attempts > 0)) {
				--attempts;

				std::ostringstream os;
				os << action_result;
				logger->write(error, "Error arming system " + std::to_string(system_id) + ": " + os.str());

				std::this_thread::sleep_for(refresh_time);

				action_result = sp.action->arm();
			}

			if (action_result == Action::Result::Success) {
				logger->write(info, "System " + std::to_string(system_id) + " armed");
				sync_point.arrive_and_wait();
			} else {
				float percentage_remaining_systems;
				mut.lock();
				--final_systems;
				percentage_remaining_systems = 100.0F * final_systems / expected_systems_float;
				if (percentage_remaining_systems < percentage_drones_required) {
					operation_ok = false;
					error_code = ProRetCod::ACTION_FAILURE;
				}
				mut.unlock();
				logger->write(warning, "System " + std::to_string(system_id) + " discarded. Remaining " + std::to_string(percentage_remaining_systems) + "%");
				sync_point.arrive_and_drop();
				return;
			}

			// Takeoff
			mut.lock();
			operation_name = "take off";
			mut.unlock();

			logger->write(info, "Taking off system " + std::to_string(system_id));

			action_result = sp.action->takeoff(); 
			if (action_result == Action::Result::Success) {
				logger->write(info, "System " + std::to_string(system_id) + " taking off");

				// Waiting to finish takeoff
				std::promise<void> prom;
				std::future fut{prom.get_future()};

				Telemetry::LandedStateHandle handle = sp.telemetry->subscribe_landed_state([&prom, &sp, &handle](Telemetry::LandedState land_state) {
					if (land_state == Telemetry::LandedState::InAir) {
						prom.set_value();
						sp.telemetry->unsubscribe_landed_state(handle);
					} else {
						float current_altitude{sp.telemetry->position().relative_altitude_m};
						std::ostringstream os;
						os << land_state;
						logger->write(debug, "System " + std::to_string(system_id) + " " + os.str() +
																									" - Altitude: " + std::to_string(current_altitude));
					}
				});

				fut.get();
			} else {
				std::ostringstream os;
				os << action_result;
				logger->write(error, "Error taking off system " + std::to_string(system_id) + ": " + os.str());

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

			logger->write(info, "Setting setpoint on system " + std::to_string(system_id));

			Offboard::PositionNedYaw expected_position_yaw{10.0f, 0.0f, -10.0f, 0.0f};
			Offboard::Result offboard_result{sp.offboard->set_position_ned(expected_position_yaw)}; 
			if (offboard_result == Offboard::Result::Success) {
				logger->write(info, "Setpoint of system " + std::to_string(system_id) + " set");
			} else {
				std::ostringstream os;
				os << offboard_result;
				logger->write(error, "Error setting setpoint on system " + std::to_string(system_id) + ": " + os.str());

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

			logger->write(info, "Starting offboard mode on system " + std::to_string(system_id));

			offboard_result = sp.offboard->start();
			if (offboard_result == Offboard::Result::Success) {
				logger->write(info, "System " + std::to_string(system_id) + " in offboard mode");

				Telemetry::PositionVelocityNedHandle handle {sp.telemetry->subscribe_position_velocity_ned([&waiting_mutex, &expected_position, &handle, &sp](Telemetry::PositionVelocityNed pos) {
					logger->write(debug, "System " + std::to_string(system_id) + " position & velocity:\n" +
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
						logger->write(info, "System " + std::to_string(system_id) + " has reached the target");
						waiting_mutex.unlock();
					}
				})};

				waiting_mutex.lock();

			} else {
				std::ostringstream os;
				os << offboard_result;
				logger->write(error, "Error starting offboard mode on system " + std::to_string(system_id) + ": " + os.str());

				mut.lock();
				operation_ok = false;
				error_code = ProRetCod::OFFBOARD_FAILURE;
				mut.unlock();
			}

			sync_point.arrive_and_wait();
			*/

			/*
			// Hold position
			mut.lock();
			operation_name = "hold position";
			mut.unlock();

			action_result = sp.action->hold();
			if (action_result == Action::Result::Success) {
				logger->write(info, "System " + std::to_string(system_id) + " holding position");
			} else {
				std::ostringstream os;
				os << action_result;
				logger->write(error, "Error holding position on system " + std::to_string(system_id) + ": " + os.str());

				mut.lock();
				operation_ok = false;
				error_code = ProRetCod::ACTION_FAILURE;
				mut.unlock();
			}
			*/
		}}));
	}

	for (shared_ptr<std::thread> th : threads_for_waiting) {
		th->join();
	}

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

float wait_systems(Mavsdk &mavsdk, const vector<System>::size_type expected_systems, shared_ptr<CheckEnoughSystems> enough_systems) {
	vector<System>::size_type discovered_systems {mavsdk.systems().size()};

	std::promise<void> prom{};
	std::future fut{prom.get_future()};
	unsigned int times_executed{0};

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
		logger->write(warning, "Not all systems found. " + enough_systems->get_status(static_cast<float>(discovered_systems)));

		if (enough_systems->exists_enough_systems(discovered_systems)) {
			logger->write(info, "Can continue");
		} else {
			logger->write(error, "Cannot continue");

			exit(static_cast<int>(ProRetCod::NO_SYSTEMS_FOUND));
		}
	}
	logger->write(info, "Systems search completed");

	return static_cast<float>(discovered_systems);
}