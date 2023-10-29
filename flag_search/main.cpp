/**
 * The MIT License (MIT)
 * Copyright (c) 2023 Pablo López Sedeño
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/offboard/offboard.h>
#include <mavsdk/log_callback.h>
#include "src/flag/flag.hpp"
#include "../src/Logger/include/loggerbuilder.hpp"
#include "src/poly/polygon.hpp"
#include "src/missionhelper/missionhelper.hpp"
#include "src/missioncontrol/missioncontrol.hpp"
#include "../src/operation/operation.hpp"
#include "../src/errorcontrol/error_control.hpp"
#include <thread>
#include <chrono>
#include <future>
#include <fstream>
#include <mavsdk/geometry.h>

using namespace mavsdk;
using namespace simple_logger;
using std::vector;
using std::shared_ptr;
using std::endl;

//********** Constants **********//
const std::chrono::seconds MAX_WAITING_TIME{10};
const float TAKEOFF_ALTITUDE{3.0f};
const std::chrono::seconds REFRESH_TIME{1};
const float REASONABLE_ERROR{0.3f};
const float PERCENTAGE_DRONES_REQUIRED{66.0f};
const unsigned int MAX_ATTEMPTS{10};
const float BASE_RETURN_TO_LAUNCH_ALTITUDE{10.0f};
const double SEPARATION{5.0};

//********** Operations **********//
// Check the health of the system
struct CheckSystemHealthArgs {
	unsigned int system_id;
	Telemetry *telemetry;
	CheckEnoughSystems *enough_systems;

	CheckSystemHealthArgs(unsigned int system_id, Telemetry *telemetry,
			CheckEnoughSystems *enough_systems) {
		this->system_id = system_id;
		this->telemetry = telemetry;
		this->enough_systems = enough_systems;
	}
};
ProRetCod operation_check_system_health(OperationTools &operation, CheckSystemHealthArgs *operation_args);

// Eliminates any mission on the drone
struct ClearExistingMissionsArgs {
	unsigned int system_id;
	Mission *mission;
	CheckEnoughSystems *enough_systems;

	ClearExistingMissionsArgs(unsigned int system_id, Mission *mission,
			CheckEnoughSystems *enough_systems) {
		this->system_id = system_id;
		this->mission = mission;
		this->enough_systems = enough_systems;
	}
};
ProRetCod operation_clear_existing_missions(OperationTools &operation, ClearExistingMissionsArgs *operation_args);

// Orders the drones to return when the mission is completed
struct ReturnToLaunchArgs {
	unsigned int system_id;
	Mission *mission;
	CheckEnoughSystems *enough_systems;

	ReturnToLaunchArgs(unsigned int system_id, Mission *mission,
			CheckEnoughSystems *enough_systems) {
		this->system_id = system_id;
		this->mission = mission;
		this->enough_systems = enough_systems;
	}
};
ProRetCod operation_return_to_launch(OperationTools &operation, ReturnToLaunchArgs *operation_args);

// Set the altitude that the drones should have on return
struct ReturnToLaunchAltitudeArgs {
	unsigned int system_id;
	Action *action;
	CheckEnoughSystems *enough_systems;

	ReturnToLaunchAltitudeArgs(unsigned int system_id, Action *action,
			CheckEnoughSystems *enough_systems) {
		this->system_id = system_id;
		this->action = action;
		this->enough_systems = enough_systems;
	}
};
ProRetCod operation_return_to_launch_altitude(OperationTools &operation, ReturnToLaunchAltitudeArgs *operation_args);

// Starts the mission controller
struct SetMissionControllerArgs {
	unsigned int system_id;
	Telemetry *telemetry;
	Flag *flag;
	Mission *mission;
	Action *action;
	MissionController *mission_controller;
	CheckEnoughSystems *enough_systems;
	double separation;

	SetMissionControllerArgs(unsigned int system_id, Telemetry *telemetry, Action *action,
			Mission *mission, Flag *flag, MissionController *mission_controller, CheckEnoughSystems *enough_systems, double separation) {
		this->system_id = system_id;
		this->telemetry = telemetry;
		this->action = action;
		this->mission = mission;
		this->flag = flag;
		this->mission_controller = mission_controller;
		this->enough_systems = enough_systems;
		this->separation = separation;
	}
};
ProRetCod operation_set_mission_controller(OperationTools &operation, SetMissionControllerArgs *operation_args);

// Makes the mission plan
struct MakeMissionPlanArgs {
	unsigned int system_id;
	MissionHelper *mission_helper;
	Mission::MissionPlan *mission_plan;
	CheckEnoughSystems *enough_systems;

	MakeMissionPlanArgs(unsigned int system_id, MissionHelper *mission_helper, Mission::MissionPlan *mission_plan,
						CheckEnoughSystems *enough_systems) {
		this->system_id = system_id;
		this->mission_helper = mission_helper;
		this->mission_plan = mission_plan;
		this->enough_systems = enough_systems;
	}
};
ProRetCod operation_make_mission_plan(OperationTools &operation, MakeMissionPlanArgs *operation_args);

// Uploads mission plan to drones
struct SetMissionPlanArgs {
	unsigned int system_id;
	Mission *mission;
	Mission::MissionPlan *mission_plan;
	static mutex mut;

	SetMissionPlanArgs(unsigned int system_id, Mission *mission,
			Mission::MissionPlan *mission_plan) {
		this->system_id = system_id;
		this->mission = mission;
		this->mission_plan = mission_plan;
	}
};
mutex SetMissionPlanArgs::mut{};
ProRetCod operation_set_mission_plan(OperationTools &operation, SetMissionPlanArgs *operation_args);

// Arms the system
struct ArmSystemsArgs {
	unsigned int system_id;
	Action *action;

	ArmSystemsArgs(unsigned int system_id, Action *action) {
		this->system_id = system_id;
		this->action = action;
	}
};
ProRetCod operation_arm_systems(OperationTools &operation, ArmSystemsArgs *operation_args);

// Starts the mission
struct StartMissionArgs {
	unsigned int system_id;
	Mission *mission;

	StartMissionArgs(unsigned int system_id, Mission *mission) {
		this->system_id = system_id;
		this->mission = mission;
	}
};
ProRetCod operation_start_mission(OperationTools &operation, StartMissionArgs *operation_args);

// Shows the status of the mission and waits until the mission ends
struct WaitUntilMissionEndsArgs {
	unsigned int system_id;
	Telemetry *telemetry;
	Mission *mission;

	WaitUntilMissionEndsArgs(unsigned int system_id, Telemetry *telemetry,
							Mission *mission) {
		this->system_id = system_id;
		this->telemetry = telemetry;
		this->mission = mission;
	}
};
ProRetCod operation_wait_until_mission_ends(OperationTools &operation, WaitUntilMissionEndsArgs *operation_args);
//******************************************//

//********** Functions **********//
void establish_connections(int argc, char *argv[], Mavsdk &mavsdk);
float wait_systems(Mavsdk &mavsdk, const vector<System>::size_type expected_systems,
					CheckEnoughSystems *enough_systems);

void drone_handler(shared_ptr<System> system, Operation &operation,
					MissionHelper *mission_helper, CheckEnoughSystems *enough_systems,
					Flag *flag, double separation);

//********** Logger global variables **********//
Logger *logger_ptr;

Critical critical;
Error error;
Warning warning;
Info info;
Debug debug;

int main(int argc, char *argv[]) {
	// Logger configuration //
	TimedLoggerDecoration logger_decoration;
	UserCustomGreeter custom_greeter{[](const string &m) {
		HourLoggerDecoration decoration;

		return "[" + decoration.get_decoration() + "Greetings] " + m;
	}};

	StandardLoggerBuilder standard_logger_builder;
	standard_logger_builder.set_decoration(&logger_decoration);
	Logger *standard_logger{standard_logger_builder.build()};
	ThreadLogger thread_standard_logger{standard_logger};

	std::ofstream last_execution_stream{"logs/last_execution.log"};
	StreamLoggerBuilder stream_logger_builder{&last_execution_stream};
	stream_logger_builder.set_decoration(&logger_decoration);
	Logger *last_execution_logger{stream_logger_builder.build()};
	ThreadLogger thread_last_execution_logger{last_execution_logger};

	std::ofstream history_stream{"logs/history.log", std::ios_base::app};
	stream_logger_builder.set_stream(&history_stream)
						 .set_greeter(&custom_greeter);
	Logger *history_logger{stream_logger_builder.build()};
	ThreadLogger thread_history_logger{history_logger};

	BiLogger stream_loggers{&thread_last_execution_logger, &thread_history_logger};

	logger_ptr = new BiLogger{&thread_standard_logger, &stream_loggers};

	Logger &logger{*logger_ptr};

	// Logger filter
	UserCustomFilter filter{[](const Level &level) {
		return level > debug;
	}};

	logger.set_level_filter(&filter);

	// Disabling the MAVSDK logger //
	log::subscribe([](log::Level,   // message severity level
                          const std::string&, // message text
                          const std::string&,    // source file from which the message was sent
                          int) {                 // line number in the source file
		// returning true from the callback disables printing the message to stdout
		//return level < log::Level::Warn;
		return true;
		//return false;
	});

	if (argc < 2) {
		logger << error << "Use: " << argv[0] << " <port1> <port2> ..." << endl;

		exit(BadArgument::code);
	}


	// Constants //
	const vector<System>::size_type expected_systems{static_cast<unsigned long>(argc - 1)};

	Mavsdk mavsdk;

	// Creating the flag and search area //
	geometry::CoordinateTransformation coordinate_transformation{{47.3978409, 8.5456286}};

	/*
	geometry::CoordinateTransformation::GlobalCoordinate global_coordinate_south_west;
	geometry::CoordinateTransformation::GlobalCoordinate global_coordinate_north_east;
	global_coordinate_south_west = coordinate_transformation.global_from_local({-40, -40});
	global_coordinate_north_east = coordinate_transformation.global_from_local({30, 30});
	RandomFlag::MaxMin latitude_deg{global_coordinate_south_west.latitude_deg, global_coordinate_north_east.latitude_deg};
	RandomFlag::MaxMin longitude_deg{global_coordinate_south_west.longitude_deg, global_coordinate_north_east.longitude_deg};
	RandomFlag flag{latitude_deg, longitude_deg};
	Polygon search_area;
	search_area.push_back({latitude_deg.get_min(), longitude_deg.get_min()});
	search_area.push_back({latitude_deg.get_min(), longitude_deg.get_max()});
	search_area.push_back({latitude_deg.get_max(), longitude_deg.get_max()});
	search_area.push_back({latitude_deg.get_max(), longitude_deg.get_min()});
	*/

	geometry::CoordinateTransformation::GlobalCoordinate global_coordinate;
	Polygon search_area;
	/*
	global_coordinate = coordinate_transformation.global_from_local({0, 0});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({0, 90});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({20, 90});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({20, 0});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	*/
	/*
	global_coordinate = coordinate_transformation.global_from_local({-10, -10});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({-10, 10});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({10, 10});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({10, -10});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	*/
	global_coordinate = coordinate_transformation.global_from_local({-40, -40});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({-40, 30});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({30, 30});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({30, -40});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	/*
	global_coordinate = coordinate_transformation.global_from_local({-40, 0});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({0, 30});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({30, 0});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({0, -40});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	*/
	/*
	global_coordinate = coordinate_transformation.global_from_local({-20, 10});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({-10, 30});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({7.5, 30});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({15, 10});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({22.5, 20});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({30, -10});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({-25, -10});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({-40, -5});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({-30, 20});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	*/

	//FixedFlag flag{Flag::Position{47.397953, 8.545955}}; // Encuentra para un rectángulo de 20x90
	//FixedFlag flag{Flag::Position{47.397868, 8.545665}}; // Encuentra para un rectángulo de 20x90
	//FixedFlag flag{Flag::Position{100, 100}};
	//FixedFlag flag{Flag::Position{47.397586, 8.5455620}};
	FixedFlag flag{Flag::Position{47.397637, 8.545618}};

	//RandomFlagPoly flag{search_area};

	logger << debug << "The flag is in:\n" << static_cast<string>(flag) << endl;

	logger.write(debug, "Search area:");
	for (auto v : search_area.get_vertices()) {
		logger << debug << "    " << v << endl;
	}

	// Setting the missionhelper //
	geometry::CoordinateTransformation::GlobalCoordinate base{coordinate_transformation.global_from_local({0, 0})};
	geometry::CoordinateTransformation::GlobalCoordinate separation{coordinate_transformation.global_from_local({SEPARATION, 0})};
	logger << debug << "Separation: " << (separation.latitude_deg - base.latitude_deg) << endl;
	ParallelSweep mission_helper{search_area, separation.latitude_deg - base.latitude_deg};

	// Setting the systems counter //
	PercentageCheck enough_systems{static_cast<float>(expected_systems), PERCENTAGE_DRONES_REQUIRED};

	establish_connections(argc, argv, mavsdk);
	float final_systems{wait_systems(mavsdk, expected_systems, &enough_systems)};

	for (shared_ptr<System> s : mavsdk.systems()) {
		logger << debug << "System: " << s->get_system_id() << "\n" << std::boolalpha
			<< "    Is connected: " << s->is_connected() << "\n"
			<< "    Has autopilot: " << s->has_autopilot() << "\n" << endl;
	}

	vector<std::thread> threads_for_waiting{};

	// Defining the Operation object //
	OperationTools operation_tools;
	std::function<void()> sync_handler{
		[&operation_tools, &logger]() {
			OkCode ok_code;

			if (operation_tools.is_critical()) {
				logger << critical << "Operation \"" << operation_tools.get_name() << "\" fails" << endl;
				exit(operation_tools.get_status_code().get_code());
			} else if (operation_tools.get_status_code() == ok_code) {
				logger << info << "Synchronization point: " << operation_tools.get_name() << endl;
			} else {
				logger << error << "Synchronization point: " << operation_tools.get_name()
					<< " -- some error has ocurred: " << operation_tools.get_status_code().get_string() << endl;
			}
		}
	};
	std::barrier<std::function<void()>> sync_point{static_cast<std::ptrdiff_t>(final_systems), sync_handler};

	Operation operation{operation_tools, &sync_point};

	// Starting threads //
	for (shared_ptr<System> system : mavsdk.systems()) {
		threads_for_waiting.push_back(
			std::thread{drone_handler, system, std::ref(operation),
							&mission_helper, &enough_systems, &flag,
							separation.latitude_deg - base.latitude_deg}
		);
	}

	// Waiting threads //
	for (std::thread &th : threads_for_waiting) {
		th.join();
	}

	delete logger_ptr;
	delete standard_logger;
	delete last_execution_logger;
	delete history_logger;

	return static_cast<int>(OkCode::code);
}

void establish_connections(int argc, char *argv[], Mavsdk &mavsdk) {
	Logger &logger{*logger_ptr};
	// Store the URLs to access the drones
	vector<std::string> url_list{};

	for (int i {1}; i < argc; ++i) {
		url_list.push_back("udp://:" + std::string {argv[i]});
	}

	logger.write(info, "Establishing connection...");
	for (std::string url : url_list) {
		ConnectionResult connection_result{mavsdk.add_any_connection(url)};
		if (connection_result == ConnectionResult::Success) {
			logger.write(info, "Connection established on " + url);
		} else {
			logger << critical << "Connection failed on " << url << ": " << connection_result << endl;

			exit(ConnectionFailed::code);
		}
	}
}

float wait_systems(Mavsdk &mavsdk, const vector<System>::size_type expected_systems, CheckEnoughSystems *enough_systems) {
	Logger &logger{*logger_ptr};

	vector<System>::size_type discovered_systems {mavsdk.systems().size()};

	std::promise<void> prom{};
	std::future fut{prom.get_future()};
	unsigned int times_executed{0};

	mavsdk.subscribe_on_new_system([&logger, &mavsdk, &discovered_systems, expected_systems, &prom, &times_executed, &enough_systems]() {
		discovered_systems = mavsdk.systems().size();
		vector<System>::size_type remaining_systems {expected_systems - discovered_systems};

		shared_ptr<System> s {mavsdk.systems().back()};
		logger << info << "Systems found: " << discovered_systems << endl;
		logger << info << "Remaining systems: " << remaining_systems << endl;

		++times_executed;

		if (times_executed == expected_systems) {
			mavsdk.subscribe_on_new_system(nullptr);
			prom.set_value();
		}
	});

	std::future_status future_status{fut.wait_for(MAX_WAITING_TIME)};
	enough_systems->append_system(static_cast<float>(discovered_systems));
	if (future_status != std::future_status::ready) {
		logger.write(warning, "Not all systems found. " + enough_systems->get_status());

		if (enough_systems->exists_enough_systems()) {
			logger.write(info, "Can continue");
		} else {
			logger.write(critical, "Cannot continue");

			exit(static_cast<int>(NoSystemsFound::code));
		}
	}
	logger.write(info, "Systems search completed");

	return static_cast<float>(discovered_systems);
}

ProRetCod operation_check_system_health(OperationTools &operation, CheckSystemHealthArgs *args) {
	Logger &logger{*logger_ptr};

	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger << info << "Checking system " << args->system_id << endl;

	bool all_ok{args->telemetry->health_all_ok()}; 
	unsigned int attempts{MAX_ATTEMPTS};
	while ((not all_ok) and (attempts > 0)) {
		--attempts;

		logger << warning << "Some failure has occurred in the system " << args->system_id
						<< ". Remaining attempts: " << attempts << endl;
		Telemetry::Health health{args->telemetry->health()};

		logger << warning << "System " << args->system_id << "\n" << std::boolalpha
			<< "    is accelerometer calibration OK: " << health.is_accelerometer_calibration_ok << "\n"
			<< "    is armable: " << health.is_armable << "\n"
			<< "    is global position OK: " << health.is_global_position_ok << "\n"
			<< "    is gyrometer calibration OK: " << health.is_gyrometer_calibration_ok << "\n"
			<< "    is home position OK: " << health.is_home_position_ok << "\n"
			<< "    is local position OK: " << health.is_local_position_ok << "\n"
			<< "    is magnetometer calibration OK: " << health.is_magnetometer_calibration_ok
			<< endl;

		std::this_thread::sleep_for(REFRESH_TIME);

		all_ok = args->telemetry->health_all_ok(); 
	}

	if (all_ok) {
		logger << info << "All OK in system " << args->system_id << endl;
	} else {
		args->enough_systems->subtract_system();
		TelemetryFailure failure;
		ret = failure;
		operation.set_failure(failure, not args->enough_systems->exists_enough_systems());
		logger << error << "System " << args->system_id << " discarded. " << args->enough_systems->get_status() << endl;
	}

	return ret;
}

ProRetCod operation_clear_existing_missions(OperationTools &operation, ClearExistingMissionsArgs *args) {
	Logger &logger{*logger_ptr};

	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger << info << "System " << args->system_id << " clearing existing missions" << endl;

	Mission::Result mission_result{args->mission->clear_mission()}; 
	unsigned int attempts{MAX_ATTEMPTS};
	while ((mission_result != Mission::Result::Success) and (attempts > 0)) {
		--attempts;

		logger << warning << "Error clearing existing missions on system " << args->system_id << ": "
			<< mission_result << ". Remaining attempts: " << attempts << endl;

		std::this_thread::sleep_for(REFRESH_TIME);

		mission_result = args->mission->clear_mission(); 
	}

	if (mission_result == Mission::Result::Success) {
		logger << info << "System " << args->system_id << " clean" << endl;
	} else {
		args->enough_systems->subtract_system();
		MissionFailure failure;
		ret = failure;
		operation.set_failure(failure, not args->enough_systems->exists_enough_systems());
		logger << error << "System " << args->system_id << " discarded. " << args->enough_systems->get_status() << endl;
	}

	return ret;
}

ProRetCod operation_return_to_launch(OperationTools &operation, ReturnToLaunchArgs *args) {
	Logger &logger{*logger_ptr};
	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger << info << "System " << args->system_id << " set return to launch after mission true" << endl;

	Mission::Result mission_result{args->mission->set_return_to_launch_after_mission(true)}; 
	unsigned int attempts{MAX_ATTEMPTS};
	while ((mission_result != Mission::Result::Success) and (attempts > 0)) {
		--attempts;

		logger << warning << "Error in setting the return to launch after mission on system "
			<< args->system_id << ": " << mission_result << ". Remaining attempts: " << std::to_string(attempts) << endl;

		std::this_thread::sleep_for(REFRESH_TIME);

		mission_result = args->mission->set_return_to_launch_after_mission(true); 
	}

	if (mission_result == Mission::Result::Success) {
		logger << info << "System " << args->system_id << " ready" << endl;
	} else {
		args->enough_systems->subtract_system();
		MissionFailure failure;
		ret = failure;
		operation.set_failure(failure, not args->enough_systems->exists_enough_systems());
		logger << error << "System " << args->system_id << " discarded. " << args->enough_systems->get_status() << endl;
	}

	return ret;
}

ProRetCod operation_return_to_launch_altitude(OperationTools &operation, ReturnToLaunchAltitudeArgs *args) {
	Logger &logger{*logger_ptr};

	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger << info << "System " << args->system_id << " set return to launch altitude" << endl;

	Action::Result action_result{args->action->set_return_to_launch_altitude(BASE_RETURN_TO_LAUNCH_ALTITUDE + static_cast<float>(args->system_id))}; 
	unsigned int attempts{MAX_ATTEMPTS};
	while ((action_result != Action::Result::Success) and (attempts > 0)) {
		--attempts;

		logger << warning << "Error in setting the return to launch altitude on system " << args->system_id
			<< ": " << action_result << ". Remaining attempts: " << attempts << endl;

		std::this_thread::sleep_for(REFRESH_TIME);

		action_result = args->action->set_return_to_launch_altitude(BASE_RETURN_TO_LAUNCH_ALTITUDE + static_cast<float>(args->system_id)); 
	}

	if (action_result == Action::Result::Success) {
		logger << info << "System " << args->system_id << " return to launch altitude ready" << endl;
	} else {
		args->enough_systems->subtract_system();
		
		ActionFailure failure;
		ret = failure;
		operation.set_failure(failure, not args->enough_systems->exists_enough_systems());
		logger << error << "Error in setting the return to launch altitude on system " << args->system_id << ": " << action_result << endl;
		logger << error << "System " << args->system_id << " discarded. " << args->enough_systems->get_status() << endl;
	}

	return ret;
}

ProRetCod operation_set_mission_controller(OperationTools &operation, SetMissionControllerArgs *args) {
	Logger &logger{*logger_ptr};

	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger << info << "System " << args->system_id << " set mission controller" << endl;

	MissionControllerStatus mission_controller_status{args->mission_controller->mission_control()};
	MCSSuccess mcs_success;
	unsigned int attempts{MAX_ATTEMPTS};
	while((mission_controller_status != mcs_success) and (attempts > 0)) {
		--attempts;

		logger << warning << "Error setting the mission controller in system "
			<< args->system_id << ": " << mission_controller_status.get_string()
			<< ". Remaining attempts: " << attempts << endl;
	
		std::this_thread::sleep_for(REFRESH_TIME);

		mission_controller_status = args->mission_controller->mission_control();
	}

	if (mission_controller_status == mcs_success) {
		logger << info << "System " << args->system_id << " mission controller ready" << endl;
	} else {
		args->enough_systems->subtract_system();
		MissionFailure failure;
		ret = failure;
		operation.set_failure(failure, not args->enough_systems->exists_enough_systems());
		logger << error << "Error setting the mission controller in system " << args->system_id << ": " << mission_controller_status.get_string() << endl;
		logger << error << "System " << args->system_id << " discarded. " << args->enough_systems->get_status() << endl;
	}

	return ret;
}

ProRetCod operation_make_mission_plan(OperationTools &operation, MakeMissionPlanArgs *args) {
	Logger &logger{*logger_ptr};
	
	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger << info << "System " << args->system_id << " making mission plan" << endl;

	vector<Mission::MissionItem> mission_item_vector;

	try {
		args->mission_helper->new_mission(args->enough_systems->get_number_of_systems(), mission_item_vector);
	} catch (const CannotMakeMission &e) {
		logger << critical << "System " << args->system_id << " cannot make a mission: " << e.what() << endl;

		ActionFailure failure;
		ret = failure;
		operation.set_failure(failure, true);
	}

	for (auto p : mission_item_vector) {
		logger << debug << "System " << args->system_id
			<< " mission. Latitude: " << p.latitude_deg
			<< ". Longitude: " << p.longitude_deg << endl;
	}

	args->mission_plan->mission_items = mission_item_vector;

	return ret;
}

ProRetCod operation_set_mission_plan(OperationTools &operation, SetMissionPlanArgs *args) {
	Logger &logger{*logger_ptr};

	OkCode ok_code;
	ProRetCod ret{ok_code};

	SetMissionPlanArgs::mut.lock();
	logger << info << "Uploading mission plan to system " << args->system_id << endl;
	std::this_thread::sleep_for(REFRESH_TIME); // Guarantees success
	Mission::Result mission_result{args->mission->upload_mission(*(args->mission_plan))};
	SetMissionPlanArgs::mut.unlock();

	unsigned int attempts{MAX_ATTEMPTS};
	while ((mission_result != Mission::Result::Success) and (attempts > 0)) {
		--attempts;

		logger << warning << "Error uploading mission plan to system "
			<< args->system_id << ": " << mission_result << ". Remaining attempts: " << attempts << endl;

		SetMissionPlanArgs::mut.lock();
		logger << info << "Uploading mission plan to system " << args->system_id << endl;
		std::this_thread::sleep_for(REFRESH_TIME);
		mission_result = args->mission->upload_mission(*(args->mission_plan));
		SetMissionPlanArgs::mut.unlock();
	}

	if (mission_result == Mission::Result::Success) {
		logger << info << "Mission plan uploaded to system " << args->system_id << endl;
	} else {
		logger << critical << "Error uploading mission plan to system " << args->system_id << ": " << mission_result << endl;

		MissionFailure failure;
		ret = failure;
		operation.set_failure(failure, true);
	}

	return ret;
}

ProRetCod operation_arm_systems(OperationTools &operation, ArmSystemsArgs *args) {
	Logger &logger{*logger_ptr};
	
	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger << info << "Arming system " << args->system_id << endl;

	Action::Result action_result{args->action->arm()};
	unsigned int attempts{MAX_ATTEMPTS};
	while ((action_result != Action::Result::Success) and (attempts > 0)) {
		--attempts;

		logger << warning << "Error arming system " << args->system_id << ": " << action_result << ". Remaining attempts: " << attempts << endl;

		std::this_thread::sleep_for(REFRESH_TIME);

		action_result = args->action->arm();
	}

	if (action_result == Action::Result::Success) {
		logger << info << "System " << args->system_id << " armed" << endl;
	} else {
		logger << critical << "System " << args->system_id << " cannot be armed" << endl;

		ActionFailure failure;
		ret = failure;
		operation.set_failure(failure, true);
	}

	return ret;
}

ProRetCod operation_start_mission(OperationTools &operation, StartMissionArgs *args) {
	Logger &logger{*logger_ptr};
	
	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger << info << "Starting mission on system " << args->system_id << endl;

	Mission::Result mission_result{args->mission->start_mission()};

	unsigned int attempts{MAX_ATTEMPTS};
	while ((mission_result != Mission::Result::Success) and (attempts > 0)) {
		--attempts;

		logger << warning << "Error starting mission on system " << args->system_id
			<< ": " << mission_result << ". Remaining attempts: " << attempts << endl;

		std::this_thread::sleep_for(REFRESH_TIME);
		
		mission_result = args->mission->start_mission();
	}

	if (mission_result == Mission::Result::Success) {
		logger << info << "The mission has started correctly in system " << args->system_id << endl;
	} else {
		logger << critical << "Error starting mission on system " << args->system_id << ": " << mission_result << endl;

		MissionFailure failure;
		ret = failure;
  		operation.set_failure(failure, true);
	}

	return ret;
}

ProRetCod operation_wait_until_mission_ends(OperationTools &, WaitUntilMissionEndsArgs *args) {
	Logger &logger{*logger_ptr};
	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger << info << "System " << args->system_id << " wating until mission ends" << endl;

	args->mission->subscribe_mission_progress([&logger, &args](Mission::MissionProgress mis_prog) {
		logger << info << "System " << args->system_id << " mission status: "
			<< mis_prog.current << "/" << mis_prog.total << endl;

		if (mis_prog.current == mis_prog.total) {
			args->mission->subscribe_mission_progress(nullptr);
		}
	});

	std::promise<void> prom_in_air;
	std::future fut_in_air{prom_in_air.get_future()};

	args->telemetry->subscribe_landed_state([&args, &prom_in_air](Telemetry::LandedState state) {
		if (state == Telemetry::LandedState::InAir) {
			args->telemetry->subscribe_landed_state(nullptr);
			prom_in_air.set_value();
		}
	});

	fut_in_air.get();

	std::promise<void> prom_on_ground;
	std::future fut_on_ground{prom_on_ground.get_future()};

	args->telemetry->subscribe_landed_state([&logger, &args, &prom_on_ground](Telemetry::LandedState state) {
		if (state == Telemetry::LandedState::OnGround) {
			logger << info << "System " << args->system_id << " on ground" << endl;
			args->telemetry->subscribe_landed_state(nullptr);
			prom_on_ground.set_value();
		}
	});

	fut_on_ground.get();

	return ret;
}

void drone_handler(shared_ptr<System> system, Operation &operation,
					MissionHelper *mission_helper,
					CheckEnoughSystems *enough_systems, Flag *flag,
					double separation) {
	Logger &logger{*logger_ptr};

	unsigned int system_id{static_cast<unsigned int>(system->get_system_id())};
	Action action{system};
	Telemetry telemetry{system};
	Mission mission{system};

	SearchController mission_controller{&telemetry, flag,
		[system_id, &logger, &action, &mission](Flag::Position flag_position, bool flag_found_by_me) {
			if (flag_found_by_me) {
			logger << info << "Flag found by system " << system_id << ": "
				<< flag_position.latitude_deg <<  ", " << flag_position.longitude_deg << endl;
			}

			action.return_to_launch_async([](mavsdk::Action::Result) {});
			mission.subscribe_mission_progress(nullptr);
		}, 1, separation
	};
	OkCode ok_code;

	// Check the health of all systems
	CheckSystemHealthArgs check_system_health_args{system_id, &telemetry, enough_systems};

	if (operation.new_operation<CheckSystemHealthArgs>("check system health", operation_check_system_health, &check_system_health_args) != ok_code) {
		logger << debug << "Ending thread " << system_id << endl;
		return;
	}

	// Clear existing missions
	ClearExistingMissionsArgs clear_existing_missions_args{system_id, &mission, enough_systems};

	if (operation.new_operation<ClearExistingMissionsArgs>("clear existing missions", operation_clear_existing_missions, &clear_existing_missions_args) != ok_code) {
		logger << debug << "Ending thread " << system_id << endl;
		return;
	}

	// Set return to launch after mission true
	ReturnToLaunchArgs return_to_launch_args{system_id, &mission, enough_systems};

	if (operation.new_operation<ReturnToLaunchArgs>("set return to launch after mission true", operation_return_to_launch, &return_to_launch_args) != ok_code) {
		logger << debug << "Ending thread " << system_id << endl;
		return;
	}

	// Set return to launch altitude
	ReturnToLaunchAltitudeArgs return_to_launch_altitude_args{system_id, &action, enough_systems};

	if (operation.new_operation<ReturnToLaunchAltitudeArgs>("set return to launch altitude", operation_return_to_launch_altitude, &return_to_launch_altitude_args) != ok_code) {
		logger << debug << "Ending thread " << system_id << endl;
		return;
	}

	// Set mission controller
	SetMissionControllerArgs set_mission_controller_args{system_id, &telemetry, &action, &mission,
															flag, &mission_controller, enough_systems, separation};
	
	if (operation.new_operation<SetMissionControllerArgs>("set mission controller", operation_set_mission_controller, &set_mission_controller_args) != ok_code) {
		logger << debug << "Ending thread " << system_id << endl;
		return;
	}

	// Make mission plan
	Mission::MissionPlan mission_plan;
	MakeMissionPlanArgs make_mission_plan_args{system_id, mission_helper, &mission_plan, enough_systems};

	if (operation.new_operation<MakeMissionPlanArgs>("make mission plan", operation_make_mission_plan, &make_mission_plan_args) != ok_code) {
		logger << debug << "Ending thread " << system_id << endl;
		return;
	}

	// Set mission plan
	SetMissionPlanArgs set_mission_plan_args{system_id, &mission, &mission_plan};

	if (operation.new_operation<SetMissionPlanArgs>("set mission plan", operation_set_mission_plan, &set_mission_plan_args) != ok_code) {
		logger << debug << "Ending thread " << system_id << endl;
		return;
	}

	// Arming systems
	ArmSystemsArgs arm_systems_args{system_id, &action};

	if (operation.new_operation<ArmSystemsArgs>("arm systems", operation_arm_systems, &arm_systems_args) != ok_code) {
		logger << debug << "Ending thread " << system_id << endl;
		return;
	}
	
	// Start mission
	StartMissionArgs start_mission_args{system_id, &mission};

	if (operation.new_operation<StartMissionArgs>("start mission", operation_start_mission, &start_mission_args) != ok_code) {
		logger << debug << "Ending thread " << system_id << endl;
		return;
	}

	// Wait until the mission ends
	WaitUntilMissionEndsArgs wait_until_mission_ends_args{system_id, &telemetry, &mission};

	if (operation.new_operation<WaitUntilMissionEndsArgs>("wait until the mission ends", operation_wait_until_mission_ends, &wait_until_mission_ends_args) != ok_code) {
		logger << debug << "Ending thread " << system_id << endl;
		return;
	}
}
