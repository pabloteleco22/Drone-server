#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/offboard/offboard.h>
#include <mavsdk/log_callback.h>
#include "lib/flag/flag.hpp"
#include "lib/log/log.hpp"
#include "lib/poly/polygon.hpp"
#include "lib/missionhelper/missionhelper.hpp"
#include "lib/missioncontrol/missioncontrol.hpp"
#include "lib/operation/operation.hpp"
#include "lib/errorcontrol/error_control.hpp"
#include <thread>
#include <barrier>
#include <chrono>
#include <future>
#include <string>
#include <sstream>
#include <fstream>
#include <mavsdk/geometry.h>

using namespace mavsdk;
using namespace simple_logger;
using std::vector;
using std::shared_ptr;

// Constants
const std::chrono::seconds MAX_WAITING_TIME{10};
const float TAKEOFF_ALTITUDE{3.0f};
const std::chrono::seconds REFRESH_TIME{1};
const float REASONABLE_ERROR{0.3f};
const float PERCENTAGE_DRONES_REQUIRED{66.0f};
const unsigned int MAX_ATTEMPTS{10};
const float BASE_RETURN_TO_LAUNCH_ALTITUDE{10.0f};

//********** Operations **********//
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

struct ArmSystemsArgs {
	unsigned int system_id;
	Action *action;

	ArmSystemsArgs(unsigned int system_id, Action *action) {
		this->system_id = system_id;
		this->action = action;
	}
};
ProRetCod operation_arm_systems(OperationTools &operation, ArmSystemsArgs *operation_args);

struct StartMissionArgs {
	unsigned int system_id;
	Mission *mission;

	StartMissionArgs(unsigned int system_id, Mission *mission) {
		this->system_id = system_id;
		this->mission = mission;
	}
};
ProRetCod operation_start_mission(OperationTools &operation, StartMissionArgs *operation_args);

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

void establish_connections(int argc, char *argv[], Mavsdk &mavsdk);
float wait_systems(Mavsdk &mavsdk, const vector<System>::size_type expected_systems,
					CheckEnoughSystems *enough_systems);

void drone_handler(shared_ptr<System> system, Operation &operation,
					MissionHelper *mission_helper, CheckEnoughSystems *enough_systems,
					Flag *flag, double separation);

Logger *logger;

Critical critical;
Error error;
Warning warning;
Info info;
Debug debug;

int main(int argc, char *argv[]) {
	if (argc < 2) {
		logger->write(error, "Use: " + string(argv[0]) + " <port1> <port2> ...");

		exit(BadArgument::code);
	}

	HourLoggerDecoration logger_decoration;
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

	logger = new BiLogger{&thread_standard_logger, &stream_loggers};

	UserCustomFilter filter{[](const Level &level) {
		return level > debug;
	}};

	logger->set_level_filter(&filter);

	// Constants
	const vector<System>::size_type expected_systems{static_cast<unsigned long>(argc - 1)};

	log::subscribe([](log::Level,   // message severity level
                          const std::string&, // message text
                          const std::string&,    // source file from which the message was sent
                          int) {                 // line number in the source file
		// returning true from the callback disables printing the message to stdout
		//return level < log::Level::Warn;
		return true;
		//return false;
	});

	Mavsdk mavsdk;

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

	FixedFlag flag{Flag::Position{47.397953, 8.545955}}; // Encuentra para un rectángulo de 20x90
	//FixedFlag flag{Flag::Position{47.397868, 8.545665}}; // Encuentra para un rectángulo de 20x90
	//FixedFlag flag{Flag::Position{100, 100}};
	geometry::CoordinateTransformation::GlobalCoordinate global_coordinate;
	Polygon search_area;
	global_coordinate = coordinate_transformation.global_from_local({0, 0});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({0, 90});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({20, 90});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({20, 0});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
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
	/*
	global_coordinate = coordinate_transformation.global_from_local({-40, -40});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({-40, 30});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({30, 30});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({30, -40});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	*/

	logger->write(debug, "The flag is in:\n" + static_cast<string>(flag));

	logger->write(debug, "Search area:");
	for (auto v : search_area.get_vertex()) {
		logger->write(debug, "    " + static_cast<string>(v));
	}

	geometry::CoordinateTransformation::GlobalCoordinate base{coordinate_transformation.global_from_local({0, 0})};
	geometry::CoordinateTransformation::GlobalCoordinate separation{coordinate_transformation.global_from_local({5, 0})};
	logger->write(debug, "Separation: " + std::to_string(separation.latitude_deg - base.latitude_deg));
	SpiralSweep mission_helper{search_area, separation.latitude_deg - base.latitude_deg};

	PercentageCheck enough_systems{static_cast<float>(expected_systems), PERCENTAGE_DRONES_REQUIRED};

	establish_connections(argc, argv, mavsdk);
	float final_systems{wait_systems(mavsdk, expected_systems, &enough_systems)};

	for (shared_ptr<System> s : mavsdk.systems()) {
		logger->write(debug, "System: " + std::to_string(static_cast<int>(s->get_system_id())) + "\n" +
			"    Is connected: " + string((s->is_connected()) ? "true" : "false") + "\n" +
			"    Has autopilot: " + string((s->has_autopilot()) ? "true" : "false") + "\n"
		);
	}

	vector<std::thread> threads_for_waiting{};

	// Defining shared thread variables
	OperationTools operation_tools;
	std::function<void()> sync_handler{
		[&operation_tools]() {
			OkCode ok_code;

			if (operation_tools.is_critical()) {
				logger->write(critical, "Operation \"" + operation_tools.get_name() + "\" fails");
				exit(operation_tools.get_status_code().get_code());
			} else if (operation_tools.get_status_code() == ok_code) {
				logger->write(info, "Synchronization point: " + operation_tools.get_name());
			} else {
				logger->write(error, "Synchronization point: " + operation_tools.get_name() +
					" -- some error has ocurred: " + operation_tools.get_status_code().get_string());
			}
		}
	};
	std::barrier<std::function<void()>> sync_point{static_cast<std::ptrdiff_t>(final_systems), sync_handler};

	Operation operation{operation_tools, &sync_point};

	for (shared_ptr<System> system : mavsdk.systems()) {
		threads_for_waiting.push_back(
			std::thread{drone_handler, system, std::ref(operation),
							&mission_helper, &enough_systems, &flag,
							separation.latitude_deg - base.latitude_deg}
		);
	}

	for (std::thread &th : threads_for_waiting) {
		th.join();
	}

	delete logger;
	delete standard_logger;
	delete last_execution_logger;
	delete history_logger;

	return static_cast<int>(OkCode::code);
}

void establish_connections(int argc, char *argv[], Mavsdk &mavsdk) {
	// Store the URLs to access the drones
	vector<std::string> url_list{};

	for (int i {1}; i < argc; ++i) {
		// Identificación por el puerto
		url_list.push_back("udp://:" + std::string {argv[i]});

		// Identificación por la IP
		//url_list.push_back("udp://" + std::string {argv[i]} + ":8090");
	}

	logger->write(info, "Establishing connection...");
	for (std::string url : url_list) {
		ConnectionResult connection_result{mavsdk.add_any_connection(url)};
		if (connection_result == ConnectionResult::Success) {
			logger->write(info, "Connection established on " + url);
		} else {
			std::ostringstream os;
			os << connection_result;
			logger->write(critical, "Connection failed on " + url + ": " + os.str());

			exit(ConnectionFailed::code);
		}
	}
}

float wait_systems(Mavsdk &mavsdk, const vector<System>::size_type expected_systems, CheckEnoughSystems *enough_systems) {
	vector<System>::size_type discovered_systems {mavsdk.systems().size()};

	std::promise<void> prom{};
	std::future fut{prom.get_future()};
	unsigned int times_executed{0};

	mavsdk.subscribe_on_new_system([&mavsdk, &discovered_systems, expected_systems, &prom, &times_executed, &enough_systems]() {
		discovered_systems = mavsdk.systems().size();
		vector<System>::size_type remaining_systems {expected_systems - discovered_systems};

		shared_ptr<System> s {mavsdk.systems().back()};
		logger->write(info, "Systems found: " + std::to_string(discovered_systems));
		logger->write(info, "Remaining systems: " + std::to_string(remaining_systems));

		++times_executed;

		if (times_executed == expected_systems) {
			mavsdk.subscribe_on_new_system(nullptr);
			prom.set_value();
		}
	});

	std::future_status future_status{fut.wait_for(MAX_WAITING_TIME)};
	enough_systems->append_system(static_cast<float>(discovered_systems));
	if (future_status != std::future_status::ready) {
		logger->write(warning, "Not all systems found. " + enough_systems->get_status());

		if (enough_systems->exists_enough_systems()) {
			logger->write(info, "Can continue");
		} else {
			logger->write(critical, "Cannot continue");

			exit(static_cast<int>(NoSystemsFound::code));
		}
	}
	logger->write(info, "Systems search completed");

	return static_cast<float>(discovered_systems);
}

ProRetCod operation_check_system_health(OperationTools &operation, CheckSystemHealthArgs *args) {
	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger->write(info, "Checking system " + std::to_string(args->system_id));

	bool all_ok{args->telemetry->health_all_ok()}; 
	unsigned int attempts{MAX_ATTEMPTS};
	while ((not all_ok) and (attempts > 0)) {
		--attempts;

		logger->write(warning, "Some failure has occurred in the system " + std::to_string(args->system_id) +
						". Remaining attempts: " + std::to_string(attempts));
		Telemetry::Health health{args->telemetry->health()};

		logger->write(warning, "System " + std::to_string(args->system_id) + "\n" +
			"    is accelerometer calibration OK: " + string(health.is_accelerometer_calibration_ok ? "true" : "false") + "\n" +
			"    is armable: " + string(health.is_armable ? "true" : "false") + "\n" +
			"    is global position OK: " + string(health.is_global_position_ok ? "true" : "false") + "\n" +
			"    is gyrometer calibration OK: " + string(health.is_gyrometer_calibration_ok ? "true" : "false") + "\n" +
			"    is home position OK: " + string(health.is_home_position_ok ? "true" : "false") + "\n" +
			"    is local position OK: " + string(health.is_local_position_ok ? "true" : "false") + "\n" +
			"    is magnetometer calibration OK: " + string(health.is_magnetometer_calibration_ok ? "true" : "false")
		);

		std::this_thread::sleep_for(REFRESH_TIME);

		all_ok = args->telemetry->health_all_ok(); 
	}

	if (all_ok) {
		logger->write(info, "All OK in system " + std::to_string(args->system_id));
	} else {
		args->enough_systems->subtract_system();
		TelemetryFailure failure;
		ret = failure;
		operation.set_failure(failure, not args->enough_systems->exists_enough_systems());
		logger->write(error, "System " + std::to_string(args->system_id) + " discarded. " + args->enough_systems->get_status());
	}

	return ret;
}

ProRetCod operation_clear_existing_missions(OperationTools &operation, ClearExistingMissionsArgs *args) {
	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger->write(info, "System " + std::to_string(args->system_id) + " clearing existing missions");

	Mission::Result mission_result{args->mission->clear_mission()}; 
	unsigned int attempts{MAX_ATTEMPTS};
	while ((mission_result != Mission::Result::Success) and (attempts > 0)) {
		--attempts;

		std::ostringstream os;
		os << mission_result;
		logger->write(warning, "Error clearing existing missions on system " +
				std::to_string(args->system_id) + ": " + os.str() + ". Remaining attempts: " + std::to_string(attempts));

		std::this_thread::sleep_for(REFRESH_TIME);

		mission_result = args->mission->clear_mission(); 
	}

	if (mission_result == Mission::Result::Success) {
		logger->write(info, "System " + std::to_string(args->system_id) + " clean");
	} else {
		args->enough_systems->subtract_system();
		MissionFailure failure;
		ret = failure;
		operation.set_failure(failure, not args->enough_systems->exists_enough_systems());
		logger->write(error, "System " + std::to_string(args->system_id) + " discarded. " + args->enough_systems->get_status());
	}

	return ret;
}

ProRetCod operation_return_to_launch(OperationTools &operation, ReturnToLaunchArgs *args) {
	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger->write(info, "System " + std::to_string(args->system_id) + " set return to launch after mission true");

	Mission::Result mission_result{args->mission->set_return_to_launch_after_mission(true)}; 
	unsigned int attempts{MAX_ATTEMPTS};
	while ((mission_result != Mission::Result::Success) and (attempts > 0)) {
		--attempts;

		std::ostringstream os;
		os << mission_result;
		logger->write(warning, "Error in setting the return to launch after mission on system " +
				std::to_string(args->system_id) + ": " + os.str() + ". Remaining attempts: " + std::to_string(attempts));

		std::this_thread::sleep_for(REFRESH_TIME);

		mission_result = args->mission->set_return_to_launch_after_mission(true); 
	}

	if (mission_result == Mission::Result::Success) {
		logger->write(info, "System " + std::to_string(args->system_id) + " ready");
	} else {
		args->enough_systems->subtract_system();
		MissionFailure failure;
		ret = failure;
		operation.set_failure(failure, not args->enough_systems->exists_enough_systems());
		logger->write(error, "System " + std::to_string(args->system_id) + " discarded. " + args->enough_systems->get_status());
	}

	return ret;
}

ProRetCod operation_return_to_launch_altitude(OperationTools &operation, ReturnToLaunchAltitudeArgs *args) {
	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger->write(info, "System " + std::to_string(args->system_id) + " set return to launch altitude");

	Action::Result action_result{args->action->set_return_to_launch_altitude(BASE_RETURN_TO_LAUNCH_ALTITUDE + static_cast<float>(args->system_id))}; 
	unsigned int attempts{MAX_ATTEMPTS};
	while ((action_result != Action::Result::Success) and (attempts > 0)) {
		--attempts;

		std::ostringstream os;
		os << action_result;
		logger->write(warning, "Error in setting the return to launch altitude on system " + std::to_string(args->system_id) +
						": " + os.str() + ". Remaining attempts: " + std::to_string(attempts));

		std::this_thread::sleep_for(REFRESH_TIME);

		action_result = args->action->set_return_to_launch_altitude(BASE_RETURN_TO_LAUNCH_ALTITUDE + static_cast<float>(args->system_id)); 
	}

	if (action_result == Action::Result::Success) {
		logger->write(info, "System " + std::to_string(args->system_id) + " return to launch altitude ready");
	} else {
		args->enough_systems->subtract_system();
		
		ActionFailure failure;
		ret = failure;
		operation.set_failure(failure, not args->enough_systems->exists_enough_systems());
		logger->write(error, "System " + std::to_string(args->system_id) + " discarded. " + args->enough_systems->get_status());
	}

	return ret;
}

ProRetCod operation_set_mission_controller(OperationTools &operation, SetMissionControllerArgs *args) {
	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger->write(info, "System " + std::to_string(args->system_id) + " set mission controller");

	MissionControllerStatus mission_controller_status{args->mission_controller->mission_control()};

	if (mission_controller_status == MissionControllerStatus::SUCCESS) {
		logger->write(info, "System " + std::to_string(args->system_id) + " mission controller ready");
	} else {
		args->enough_systems->subtract_system();
		MissionFailure failure;
		ret = failure;
		operation.set_failure(failure, not args->enough_systems->exists_enough_systems());
		logger->write(error, "System " + std::to_string(args->system_id) + " discarded. " + args->enough_systems->get_status());
	}

	return ret;
}

ProRetCod operation_make_mission_plan(OperationTools &operation, MakeMissionPlanArgs *args) {
	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger->write(info, "System " + std::to_string(args->system_id) + " making mission plan");

	vector<Mission::MissionItem> mission_item_vector;

	try {
		args->mission_helper->new_mission(args->enough_systems->get_number_of_systems(), mission_item_vector);
	} catch (const CannotMakeMission &e) {
		logger->write(critical, "System " + std::to_string(args->system_id) + " cannot make a mission: " + e.what());

		ActionFailure failure;
		ret = failure;
		operation.set_failure(failure, true);
	}

	for (auto p : mission_item_vector) {
		logger->write(debug, "System " + std::to_string(args->system_id) +
		" mission. Latitude: " + std::to_string(p.latitude_deg) +
		". Longitude: " + std::to_string(p.longitude_deg));
	}

	args->mission_plan->mission_items = mission_item_vector;

	return ret;
}

ProRetCod operation_set_mission_plan(OperationTools &operation, SetMissionPlanArgs *args) {
	OkCode ok_code;
	ProRetCod ret{ok_code};

	SetMissionPlanArgs::mut.lock();
	std::this_thread::sleep_for(REFRESH_TIME); // Guarantees success
	logger->write(info, "Uploading mission plan to system " + std::to_string(args->system_id));
	Mission::Result mission_result{args->mission->upload_mission(*(args->mission_plan))};
	SetMissionPlanArgs::mut.unlock();

	unsigned int attempts{MAX_ATTEMPTS};
	while ((mission_result != Mission::Result::Success) and (attempts > 0)) {
		--attempts;

		std::ostringstream os;
		os << mission_result;
		logger->write(warning, "Error uploading mission plan to system " +
				std::to_string(args->system_id) + ": " + os.str() + ". Remaining attempts: " + std::to_string(attempts));

		SetMissionPlanArgs::mut.lock();
		std::this_thread::sleep_for(REFRESH_TIME);
		mission_result = args->mission->upload_mission(*(args->mission_plan));
		SetMissionPlanArgs::mut.unlock();
	}

	if (mission_result == Mission::Result::Success) {
		logger->write(info, "Mission plan uploaded to system " + std::to_string(args->system_id));
	} else {
		std::ostringstream os;
		os << mission_result;
		logger->write(critical, "Error uploading mission plan to system " + std::to_string(args->system_id) + ": " + os.str());

		MissionFailure failure;
		ret = failure;
		operation.set_failure(failure, true);
	}

	return ret;
}

ProRetCod operation_arm_systems(OperationTools &operation, ArmSystemsArgs *args) {
	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger->write(info, "Arming system " + std::to_string(args->system_id));

	Action::Result action_result{args->action->arm()};
	unsigned int attempts{MAX_ATTEMPTS};
	while ((action_result != Action::Result::Success) and (attempts > 0)) {
		--attempts;

		std::ostringstream os;
		os << action_result;
		logger->write(warning, "Error arming system " + std::to_string(args->system_id) + ": " + os.str() + ". Remaining attempts: " + std::to_string(attempts));

		std::this_thread::sleep_for(REFRESH_TIME);

		action_result = args->action->arm();
	}

	if (action_result == Action::Result::Success) {
		logger->write(info, "System " + std::to_string(args->system_id) + " armed");
	} else {
		logger->write(critical, "System " + std::to_string(args->system_id) + " cannot be armed");

		ActionFailure failure;
		ret = failure;
		operation.set_failure(failure, true);
	}

	return ret;
}

ProRetCod operation_start_mission(OperationTools &operation, StartMissionArgs *args) {
	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger->write(info, "Starting mission on system " + std::to_string(args->system_id));

	Mission::Result mission_result{args->mission->start_mission()};

	unsigned int attempts{MAX_ATTEMPTS};
	while ((mission_result != Mission::Result::Success) and (attempts > 0)) {
		--attempts;

		std::stringstream os;
		os << mission_result;

		logger->write(warning, "Error starting mission on system " + std::to_string(args->system_id) +
						": " + os.str() + ". Remaining attempts: " + std::to_string(attempts));

		std::this_thread::sleep_for(REFRESH_TIME);
		
		mission_result = args->mission->start_mission();
	}

	if (mission_result == Mission::Result::Success) {
		logger->write(info, "The mission has started correctly in system " + std::to_string(args->system_id));
	} else {
		std::ostringstream os;
		os << mission_result;
		logger->write(critical, "Error starting mission on system " + std::to_string(args->system_id) + ": " + os.str());

		MissionFailure failure;
		ret = failure;
  		operation.set_failure(failure, true);
	}

	return ret;
}

ProRetCod operation_wait_until_mission_ends(OperationTools &, WaitUntilMissionEndsArgs *args) {
	OkCode ok_code;
	ProRetCod ret{ok_code};

	logger->write(info, "System " + std::to_string(args->system_id) + " wating until mission ends");

	args->mission->subscribe_mission_progress([&args](Mission::MissionProgress mis_prog) {
		logger->write(info, "System " + std::to_string(args->system_id) + " mission status: " +
						std::to_string(mis_prog.current) + "/" + std::to_string(mis_prog.total));

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

	args->telemetry->subscribe_landed_state([&args, &prom_on_ground](Telemetry::LandedState state) {
		if (state == Telemetry::LandedState::OnGround) {
			logger->write(info, "System " + std::to_string(args->system_id) + " on ground");
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
	unsigned int system_id{static_cast<unsigned int>(system->get_system_id())};
	Action action{system};
	Telemetry telemetry{system};
	Mission mission{system};

	SearchController mission_controller{&telemetry, flag,
		[system_id, &action, &mission](Flag::Position flag_position, bool flag_found_by_me) {
			if (flag_found_by_me) {
			logger->write(info, "Flag found by system " + std::to_string(system_id) + ": " +
						std::to_string(flag_position.latitude_deg) +  ", " + std::to_string(flag_position.longitude_deg));
			}

			action.return_to_launch_async([](mavsdk::Action::Result) {});
			mission.subscribe_mission_progress(nullptr);
		}, 1, separation
	};
	OkCode ok_code;

	// Check the health of all systems
	CheckSystemHealthArgs check_system_health_args{system_id, &telemetry, enough_systems};

	if (operation.new_operation<CheckSystemHealthArgs>("check system health", operation_check_system_health, &check_system_health_args) != ok_code) {
		logger->write(debug, "Ending thread " + std::to_string(system_id));
		return;
	}

	// Clear existing missions
	ClearExistingMissionsArgs clear_existing_missions_args{system_id, &mission, enough_systems};

	if (operation.new_operation<ClearExistingMissionsArgs>("clear existing missions", operation_clear_existing_missions, &clear_existing_missions_args) != ok_code) {
		logger->write(debug, "Ending thread " + std::to_string(system_id));
		return;
	}

	// Set return to launch after mission true
	ReturnToLaunchArgs return_to_launch_args{system_id, &mission, enough_systems};

	if (operation.new_operation<ReturnToLaunchArgs>("set return to launch after mission true", operation_return_to_launch, &return_to_launch_args) != ok_code) {
		logger->write(debug, "Ending thread " + std::to_string(system_id));
		return;
	}

	// Set return to launch altitude
	ReturnToLaunchAltitudeArgs return_to_launch_altitude_args{system_id, &action, enough_systems};

	if (operation.new_operation<ReturnToLaunchAltitudeArgs>("set return to launch altitude", operation_return_to_launch_altitude, &return_to_launch_altitude_args) != ok_code) {
		logger->write(debug, "Ending thread " + std::to_string(system_id));
		return;
	}

	// Set mission controller
	SetMissionControllerArgs set_mission_controller_args{system_id, &telemetry, &action, &mission,
															flag, &mission_controller, enough_systems, separation};
	
	if (operation.new_operation<SetMissionControllerArgs>("set mission controller", operation_set_mission_controller, &set_mission_controller_args) != ok_code) {
		logger->write(debug, "Ending thread " + std::to_string(system_id));
		return;
	}

	// Make mission plan
	Mission::MissionPlan mission_plan;
	MakeMissionPlanArgs make_mission_plan_args{system_id, mission_helper, &mission_plan, enough_systems};

	if (operation.new_operation<MakeMissionPlanArgs>("make mission plan", operation_make_mission_plan, &make_mission_plan_args) != ok_code) {
		logger->write(debug, "Ending thread " + std::to_string(system_id));
		return;
	}

	// Set mission plan
	SetMissionPlanArgs set_mission_plan_args{system_id, &mission, &mission_plan};

	if (operation.new_operation<SetMissionPlanArgs>("set mission plan", operation_set_mission_plan, &set_mission_plan_args) != ok_code) {
		logger->write(debug, "Ending thread " + std::to_string(system_id));
		return;
	}

	// Arming systems
	ArmSystemsArgs arm_systems_args{system_id, &action};

	if (operation.new_operation<ArmSystemsArgs>("arm systems", operation_arm_systems, &arm_systems_args) != ok_code) {
		logger->write(debug, "Ending thread " + std::to_string(system_id));
		return;
	}
	
	// Start mission
	StartMissionArgs start_mission_args{system_id, &mission};

	if (operation.new_operation<StartMissionArgs>("start mission", operation_start_mission, &start_mission_args) != ok_code) {
		logger->write(debug, "Ending thread " + std::to_string(system_id));
		return;
	}

	// Wait until the mission ends
	WaitUntilMissionEndsArgs wait_until_mission_ends_args{system_id, &telemetry, &mission};

	if (operation.new_operation<WaitUntilMissionEndsArgs>("wait until the mission ends", operation_wait_until_mission_ends, &wait_until_mission_ends_args) != ok_code) {
		logger->write(debug, "Ending thread " + std::to_string(system_id));
		return;
	}
}