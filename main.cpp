#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <future>

using namespace mavsdk;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::shared_ptr;

// Constants
const std::chrono::seconds max_waiting_time {30};

// Process return code
enum class ProRetCod : int {
	OK = 0,
	BAD_ARGUMENT = 1,
	CONNECTION_FAILED = 2,
	NO_SYSTEMS_FOUND = 3,
	TELEMETRY_FAILURE = 4,
	ACTION_FAILURE = 5
};

// Struct to save a system and its telemetry and action objects
struct SystemPlugins {
	SystemPlugins(System &system) {
		this->system = &system;
		telemetry = std::make_shared<Telemetry>(system);
		action = std::make_shared<Action>(system);
	}

	System *system;
	shared_ptr<Telemetry> telemetry;
	shared_ptr<Action> action;
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
	const vector<System>::size_type expected_systems {static_cast<unsigned long>(argc - 1)};

	Mavsdk mavsdk;
	std::mutex mut;

	establish_connections(argc, argv, mavsdk);
	wait_systems(mavsdk, expected_systems);

	for (shared_ptr<System> s : mavsdk.systems()) {
		cout << "System: " << static_cast<int>(s->get_system_id()) << endl;
		cout << "    Is connected: " << std::boolalpha << s->is_connected() << endl;
		cout << "    Has autopilot: " << std::boolalpha << s->has_autopilot() << endl;
	}

	vector<SystemPlugins> system_plugins_list {};

	for (shared_ptr<System> s : mavsdk.systems()) {
		system_plugins_list.push_back(SystemPlugins(*s));
	}

	vector<shared_ptr<std::thread>> threads_for_waiting {};

	// Sets the position packet sending rate
	bool operation_ok = true;
	vector<SystemPlugins>::iterator sp {system_plugins_list.begin()};
	while ((check_operation_ok(operation_ok, mut)) and (sp != system_plugins_list.end())) {
		threads_for_waiting.push_back(std::make_unique<std::thread>(std::thread {[sp, &operation_ok, &mut]() {
			cout << "Setting rate in system " << static_cast<int>(sp->system->get_system_id()) << endl;

			Telemetry::Result result = sp->telemetry->set_rate_position(1.0); 
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
		cout << "All rates defined" <<endl;
	} else {
		exit(static_cast<int>(ProRetCod::TELEMETRY_FAILURE));
	}

//	for (SystemPlugins sp : system_plugins_list) {
//		sp.telemetry->subscribe_position([sp](Telemetry::Position pos) {
//			cout << "Position update" << endl;
//			cout << "System: " << static_cast<int>(sp.system->get_system_id()) << endl;
//			cout << "Altitude: " << pos.relative_altitude_m << " m" << endl;
//            cout << "Latitude: " << pos.latitude_deg << endl;
//            cout << "Longitude: " << pos.longitude_deg << endl << endl;
//		});
//	}

	// Check the health of all systems
	threads_for_waiting.clear();
	operation_ok = true;
	sp = system_plugins_list.begin();
	while ((check_operation_ok(operation_ok, mut)) and (sp != system_plugins_list.end())) {
		threads_for_waiting.push_back(std::make_unique<std::thread>(std::thread {[sp, &operation_ok, &mut]() {
			cout << "Checking system " << static_cast<int>(sp->system->get_system_id()) << endl;

			bool all_ok {sp->telemetry->health_all_ok()}; 
			if (all_ok) {
				cout << "All ok in system " << static_cast<int>(sp->system->get_system_id()) << endl;
			} else {
				cerr << "Not all ok in system " << static_cast<int>(sp->system->get_system_id()) << endl;
				Telemetry::Health health {sp->telemetry->health()};

				cout << "is accelerometer calibration ok: " << std::boolalpha << health.is_accelerometer_calibration_ok << endl;
				cout << "is armable: " << std::boolalpha << health.is_armable << endl;
				cout << "is global position ok: " << std::boolalpha << health.is_global_position_ok << endl;
				cout << "is gyrometer calibration ok: " << std::boolalpha << health.is_gyrometer_calibration_ok << endl;
				cout << "is home position ok: " << std::boolalpha << health.is_home_position_ok << endl;
				cout << "is local position ok: " << std::boolalpha << health.is_local_position_ok << endl;
				cout << "is magnetometer calibration ok: " << std::boolalpha << health.is_magnetometer_calibration_ok << endl;

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
		cout << "All systems ok" <<endl;
	} else {
		exit(static_cast<int>(ProRetCod::TELEMETRY_FAILURE));
	}

	// Arming systems
	threads_for_waiting.clear();
	operation_ok = true;
	sp = system_plugins_list.begin();
	while ((check_operation_ok(operation_ok, mut)) and (sp != system_plugins_list.end())) {
		threads_for_waiting.push_back(std::make_unique<std::thread>(std::thread {[sp, &operation_ok, &mut]() {
			cout << "Arming system " << static_cast<int>(sp->system->get_system_id()) << endl;

			Action::Result result {sp->action->arm()}; 
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
		cout << "All armed" <<endl;
	} else {
		exit(static_cast<int>(ProRetCod::ACTION_FAILURE));
	}

	//while (true);

	return static_cast<int>(ProRetCod::OK);
}

void establish_connections(int argc, char *argv[], Mavsdk &mavsdk) {
	// Store the URLs to access the drones
	vector<std::string> url_list {};

	for (int i {1}; i < argc; ++i) {
		url_list.push_back("udp://:" + std::string {argv[i]});
	}

	cout << "Establishing connection..." << endl;
	for (std::string url : url_list) {
		ConnectionResult connection_result {mavsdk.add_any_connection(url)};
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

	std::promise<void> prom {};
	std::future fut {prom.get_future()};
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