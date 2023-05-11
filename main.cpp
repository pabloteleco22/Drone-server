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
#include <geometry.h>

using namespace mavsdk;
using namespace simple_logger;
using std::vector;
using std::shared_ptr;

// Constants
const std::chrono::seconds max_waiting_time{10};
const float takeoff_altitude{3.0f};
const std::chrono::seconds refresh_time{1};
const float reasonable_error{0.3f};
const float percentage_drones_required{66.0f};
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

class Operation {
	bool operation_ok;
	ProRetCod error_code;
	string operation_name;
	std::mutex mut;

	public:
		Operation() {
			this->operation_ok = true;
			this->operation_name = "";
			this->error_code = ProRetCod::UNKNOWN_ERROR;
		}

		operator bool() {
			bool ok;

			mut.lock();
			ok = operation_ok;
			mut.unlock();

			return ok;
		}

		ProRetCod get_error_code() {
			ProRetCod cod;

			mut.lock();
			cod = error_code;
			mut.unlock();

			return cod;
		}

		string get_name() {
			string name;

			mut.lock();
			name = operation_name;
			mut.unlock();

			return name;
		}

		void set_failure(const ProRetCod error_code) {
			mut.lock();
			this->error_code = error_code;
			operation_ok = false;
			mut.unlock();
		}

		void set_name(const string operation_name) {
			mut.lock();
			this->operation_name = operation_name;
			mut.unlock();
		}
};

struct CheckEnoughSystems {
	virtual ~CheckEnoughSystems() {}
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
float wait_systems(Mavsdk &mavsdk, const vector<System>::size_type expected_systems, CheckEnoughSystems *enough_systems);

void drone_handler(shared_ptr<System> system, Operation &operation, std::mutex &mut,
							std::barrier<std::function<void()>> &sync_point, float &final_systems,
							MissionHelper *mission_helper, CheckEnoughSystems *enough_systems);

Logger *logger;

Error error;
Warning warning;
Info info;
Debug debug;

int main(int argc, char *argv[]) {
	if (argc < 2) {
		logger->write(error, "Use: " + string(argv[0]) + " <port1> <port2> ...");

		exit(static_cast<int>(ProRetCod::BAD_ARGUMENT));
	}

	shared_ptr<LoggerDecoration> logger_decoration{new HourLoggerDecoration};
	shared_ptr<UserCustomGreeter> custom_greeter{new UserCustomGreeter{[](const string &m) {
		HourLoggerDecoration decoration;

		return "[" + decoration.get_decoration() + "Greetings] " + m;
	}}};

	logger = new BiLogger{
		shared_ptr<Logger>{new ThreadLogger{shared_ptr<Logger>{new StandardLogger{logger_decoration}}}},
		shared_ptr<Logger>{new BiLogger{
			shared_ptr<Logger>{new ThreadLogger{shared_ptr<Logger>{new StreamLogger{std::make_shared<std::ofstream>("logs/last_execution.log"), logger_decoration}}}},
			shared_ptr<Logger>{new ThreadLogger{shared_ptr<Logger>{new StreamLogger{std::make_shared<std::ofstream>("logs/history.log", std::ios_base::app), logger_decoration, custom_greeter}}}}
		}}
	};

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
	geometry::CoordinateTransformation::GlobalCoordinate global_coordinate_south_west;
	geometry::CoordinateTransformation::GlobalCoordinate global_coordinate_north_east;
	global_coordinate_south_west = coordinate_transformation.global_from_local({0, 0});
	global_coordinate_north_east = coordinate_transformation.global_from_local({20, 90});
	RandomFlag::MaxMin latitude_deg{global_coordinate_south_west.latitude_deg, global_coordinate_north_east.latitude_deg};
	RandomFlag::MaxMin longitude_deg{global_coordinate_south_west.longitude_deg, global_coordinate_north_east.longitude_deg};
	Flag *flag{new RandomFlag{latitude_deg, longitude_deg}};
	//Flag *flag{new FixedFlag{}};

	Polygon search_area;
	search_area.push_back({latitude_deg.get_min(), longitude_deg.get_min()});
	search_area.push_back({latitude_deg.get_min(), longitude_deg.get_max()});
	search_area.push_back({latitude_deg.get_max(), longitude_deg.get_max()});
	search_area.push_back({latitude_deg.get_max(), longitude_deg.get_min()});

	/*
	geometry::CoordinateTransformation::GlobalCoordinate global_coordinate;
	global_coordinate = coordinate_transformation.global_from_local({0, 0});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({0, 90});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({20, 90});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	global_coordinate = coordinate_transformation.global_from_local({20, 0});
	search_area.push_back({global_coordinate.latitude_deg, global_coordinate.longitude_deg});
	*/

	logger->write(debug, "Search area:");
	for (auto v : search_area.get_vertex()) {
		logger->write(debug, "    " + static_cast<string>(v));
	}

	MissionHelper *mission_helper{new ParallelSweep{search_area}};

	logger->write(info, "The flag is in:\n" + static_cast<string>(*flag));

	CheckEnoughSystems *enough_systems{new PercentageCheck{static_cast<float>(expected_systems), percentage_drones_required}};

	establish_connections(argc, argv, mavsdk);
	float final_systems{wait_systems(mavsdk, expected_systems, enough_systems)};

	for (shared_ptr<System> s : mavsdk.systems()) {
		logger->write(debug, "System: " + std::to_string(static_cast<int>(s->get_system_id())) + "\n" +
			"    Is connected: " + string((s->is_connected()) ? "true" : "false") + "\n" +
			"    Has autopilot: " + string((s->has_autopilot()) ? "true" : "false") + "\n"
		);
	}

	vector<std::thread> threads_for_waiting{};

	// Defining shared thread variables
	std::mutex mut;
	Operation operation;
	std::function<void()> sync_handler{[&operation]() {
		if (operation) {
			logger->write(info, "Synchronization point: " + operation.get_name());
		} else {
			logger->write(error, "Operation \"" + operation.get_name() + "\" fails");
			exit(static_cast<int>(operation.get_error_code()));
		}
	}};
	std::barrier<std::function<void()>> sync_point{static_cast<std::ptrdiff_t>(final_systems), sync_handler};


	for (shared_ptr<System> system : mavsdk.systems()) {
		threads_for_waiting.push_back(
			std::thread{drone_handler, system, std::ref(operation), std::ref(mut),
							std::ref(sync_point), std::ref(final_systems), mission_helper, enough_systems}
		);

		std::this_thread::sleep_for(refresh_time);
	}

	for (std::thread &th : threads_for_waiting) {
		th.join();
	}

	delete logger;
	delete enough_systems;
	delete mission_helper;
	delete flag;

	return static_cast<int>(ProRetCod::OK);
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
			logger->write(error, "Connection failed on " + url + ": " + os.str());

			exit(static_cast<int>(ProRetCod::CONNECTION_FAILED));
		}
	}
}

float wait_systems(Mavsdk &mavsdk, const vector<System>::size_type expected_systems, CheckEnoughSystems *enough_systems) {
	vector<System>::size_type discovered_systems {mavsdk.systems().size()};

	std::promise<void> prom{};
	std::future fut{prom.get_future()};
	unsigned int times_executed{0};

	mavsdk.subscribe_on_new_system([&mavsdk, &discovered_systems, expected_systems, &prom, &times_executed]() {
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

void drone_handler(shared_ptr<System> system, Operation &operation, std::mutex &mut,
							std::barrier<std::function<void()>> &sync_point, float &final_systems,
							MissionHelper *mission_helper, CheckEnoughSystems *enough_systems) {
	unsigned int system_id{static_cast<unsigned int>(system->get_system_id())};
	Telemetry telemetry{system};
	Action action{system};
	Mission mission{system};

	// Check the health of all systems
	operation.set_name("check system health");

	logger->write(info, "Checking system " + std::to_string(system_id));

	bool all_ok{telemetry.health_all_ok()}; 
	unsigned int attempts{max_attempts};
	while ((not all_ok) and (attempts > 0)) {
		--attempts;

		logger->write(error, "Not all OK in system " + std::to_string(system_id) + ". Remaining attempts: " + std::to_string(attempts));
		Telemetry::Health health{telemetry.health()};

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

		all_ok = telemetry.health_all_ok(); 
	}

	if (all_ok) {
		logger->write(info, "All OK in system " + std::to_string(system_id));
		sync_point.arrive_and_wait();
	} else {
		mut.lock();
		--final_systems;
		if (not enough_systems->exists_enough_systems(final_systems)) {
			operation.set_failure(ProRetCod::TELEMETRY_FAILURE);
		}
		mut.unlock();
		logger->write(warning, "System " + std::to_string(system_id) + " discarded. " + enough_systems->get_status(final_systems));
		sync_point.arrive_and_drop();
		return;
	}

	// Clear existing missions
	operation.set_name("clear existing missions");

	logger->write(info, "System " + std::to_string(system_id) + " clearing existing missions");

	Mission::Result mission_result{mission.clear_mission()}; 
	attempts = max_attempts;
	while ((mission_result != Mission::Result::Success) and (attempts > 0)) {
		--attempts;

		std::ostringstream os;
		os << mission_result;
		logger->write(error, "Error clearing existing missions on system " + std::to_string(system_id) + ": " + os.str());

		std::this_thread::sleep_for(refresh_time);

		mission_result = mission.clear_mission(); 
	}

	if (mission_result == Mission::Result::Success) {
		logger->write(info, "System " + std::to_string(system_id) + " clean");
		sync_point.arrive_and_wait();
	} else {
		mut.lock();
		--final_systems;
		if (not enough_systems->exists_enough_systems(final_systems)) {
			operation.set_failure(ProRetCod::MISSION_FAILURE);
		}
		mut.unlock();
		logger->write(warning, "System " + std::to_string(system_id) + " discarded. " + enough_systems->get_status(final_systems));
		sync_point.arrive_and_drop();
		return;
	}

	// Set return to launch after mission true
	operation.set_name("set return to launch after mission true");

	logger->write(info, "System " + std::to_string(system_id) + " set return to launch after mission true");

	mission_result = mission.set_return_to_launch_after_mission(true); 
	attempts = max_attempts;
	while ((mission_result != Mission::Result::Success) and (attempts > 0)) {
		--attempts;

		std::ostringstream os;
		os << mission_result;
		logger->write(error, "Error in setting the return to launch after mission on system " + std::to_string(system_id) + ": " + os.str());

		std::this_thread::sleep_for(refresh_time);

		mission_result = mission.set_return_to_launch_after_mission(true); 
	}

	if (mission_result == Mission::Result::Success) {
		logger->write(info, "System " + std::to_string(system_id) + " ready");
		sync_point.arrive_and_wait();
	} else {
		mut.lock();
		--final_systems;
		if (not enough_systems->exists_enough_systems(final_systems)) {
			operation.set_failure(ProRetCod::MISSION_FAILURE);
		}
		mut.unlock();
		logger->write(warning, "System " + std::to_string(system_id) + " discarded. " + enough_systems->get_status(final_systems));
		sync_point.arrive_and_drop();
		return;
	}

	// Make mission plan
	operation.set_name("make mission plan");

	vector<Mission::MissionItem> mission_item_vector;

	try {
		mission_helper->new_mission(system_id, final_systems, mission_item_vector);
	} catch (const CannotMakeMission &e) {
		if (std::string(e.what()) == "CannotMakeMission: The system ID must be less than or equal to the number of systems") {
			mission_helper->new_mission(final_systems, final_systems, mission_item_vector);
		} else {
			logger->write(error, "System " + std::to_string(system_id) + " cannot make a mission: " + e.what());

			operation.set_failure(ProRetCod::ACTION_FAILURE);
		}
	}

	for (auto p : mission_item_vector) {
		logger->write(debug, "System " + std::to_string(system_id) + " mission.\n    Latitude: " + std::to_string(p.latitude_deg) +
		"\n    Longitude: " + std::to_string(p.longitude_deg));
	}

	Mission::MissionPlan mission_plan{mission_item_vector};

	sync_point.arrive_and_wait();

	// Set mission plan
	operation.set_name("set mission plan");

	mut.lock();
	std::this_thread::sleep_for(refresh_time); // Guarantees success
	logger->write(info, "Uploading mission plan to system " + std::to_string(system_id));
	mission_result = mission.upload_mission(mission_plan);
	mut.unlock();

	while ((mission_result != Mission::Result::Success) and (attempts > 0)) {
		--attempts;

		std::ostringstream os;
		os << mission_result;
		logger->write(error, "Error uploading mission plan to system " + std::to_string(system_id) + ": " + os.str() + ". Remaining attempts: " + std::to_string(attempts));

		mut.lock();
		std::this_thread::sleep_for(refresh_time);
		mission_result = mission.upload_mission(mission_plan);
		mut.unlock();
	}

	if (mission_result == Mission::Result::Success) {
		logger->write(info, "Mission plan uploaded to system " + std::to_string(system_id));
	} else {
		std::ostringstream os;
		os << mission_result;
		logger->write(error, "Error uploading mission plan to system " + std::to_string(system_id) + ": " + os.str());

		operation.set_failure(ProRetCod::MISSION_FAILURE);
	}

	sync_point.arrive_and_wait();

	// Arming systems
	operation.set_name("arm systems");

	logger->write(info, "Arming system " + std::to_string(system_id));

	Action::Result action_result{action.arm()}; 
	attempts = max_attempts;
	while ((action_result != Action::Result::Success) and (attempts > 0)) {
		--attempts;

		std::ostringstream os;
		os << action_result;
		logger->write(error, "Error arming system " + std::to_string(system_id) + ": " + os.str());

		std::this_thread::sleep_for(refresh_time);

		action_result = action.arm();
	}

	if (action_result == Action::Result::Success) {
		logger->write(info, "System " + std::to_string(system_id) + " armed");
	} else {
		logger->write(warning, "System " + std::to_string(system_id) + " cannot be armed");

		operation.set_failure(ProRetCod::ACTION_FAILURE);
	}

	sync_point.arrive_and_wait();
	
	// Start mission
	operation.set_name("start mission");

	mission_result = mission.start_mission();

	attempts = max_attempts;
	while ((mission_result != Mission::Result::Success) and (attempts > 0)) {
		--attempts;

		std::stringstream os;
		os << mission_result;

		logger->write(error, "Error starting mission on system " + std::to_string(system_id) + ": " + os.str() + ". Remaining attempts: " + std::to_string(attempts));

		std::this_thread::sleep_for(refresh_time);
		
		mission_result = mission.start_mission();
	}

	if (mission_result == Mission::Result::Success) {
		logger->write(info, "The mission has started correctly in system " + std::to_string(system_id));
	} else {
		std::ostringstream os;
		os << mission_result;
		logger->write(error, "Error starting mission on system " + std::to_string(system_id) + ": " + os.str());

  		operation.set_failure(ProRetCod::MISSION_FAILURE);
	}

	sync_point.arrive_and_wait();

	// Wait until the mission ends
	operation.set_name("wait until the mission ends");

	std::promise<void> prom;
	std::future fut{prom.get_future()};

	mission.subscribe_mission_progress([&system_id, &prom, &mission](Mission::MissionProgress mis_prog) {
		logger->write(info, "System " + std::to_string(system_id) + " mission status: " + std::to_string(mis_prog.current) + "/" + std::to_string(mis_prog.total));

		if (mis_prog.current == mis_prog.total) {
			mission.subscribe_mission_progress(nullptr);
			prom.set_value();
		}
	});

	fut.get();

	sync_point.arrive_and_wait();
}