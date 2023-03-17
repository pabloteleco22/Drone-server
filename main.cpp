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

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cerr << "Use: " << "<port1> <port2> ..." << endl;

		exit(static_cast<int>(ProRetCod::BAD_ARGUMENT));
	}

	// Constants
	const vector<System>::size_type expected_systems {static_cast<unsigned long>(argc - 1)};
	const unsigned int max_attempts {30};

	// Store the URLs to access the drones
	vector<std::string> url_list {};

	for (int i {1}; i < argc; ++i) {
		url_list.push_back("udp://:" + std::string {argv[i]});
	}

	Mavsdk mavsdk;

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

	vector<System>::size_type discovered_systems {mavsdk.systems().size()};

	mavsdk.subscribe_on_new_system([&mavsdk, &discovered_systems, &expected_systems]() {
		discovered_systems = mavsdk.systems().size();

		shared_ptr<System> s {mavsdk.systems().back()};
		cout << "New system!" << endl;
		cout << "System: " << static_cast<int>(s->get_system_id()) << endl;
		cout << "    Is connected: " << std::boolalpha << s->is_connected() << endl;
		cout << "    Has autopilot: " << std::boolalpha << s->has_autopilot() << endl;

		cout << "Systems found: " << discovered_systems << endl;
		cout << "Remaining systems: " << expected_systems - discovered_systems << endl;
		});
	
	unsigned int attempt {0};
	while ((attempt < max_attempts) and (discovered_systems < expected_systems)) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		++attempt;
	}

	if (discovered_systems < expected_systems) {
		cout << "Not all systems found" << endl;
		exit(static_cast<int>(ProRetCod::NO_SYSTEMS_FOUND));
	} else {
		cout << "All systems found" << endl;
	}

	vector<shared_ptr<SystemPlugins>> system_plugins_list {};

	for (shared_ptr<System> s : mavsdk.systems()) {
		shared_ptr<SystemPlugins> system_plugins = std::make_shared<SystemPlugins>(*s);
		system_plugins_list.push_back(system_plugins);
	}

	unsigned int sys_rate_correct = 0;
	std::promise<Telemetry::Result> prom {};
	std::future fut = prom.get_future();
	//for (shared_ptr<SystemPlugins> sp : system_plugins_list) {
	vector<shared_ptr<SystemPlugins>>::iterator it = system_plugins_list.begin();

	while ((fut.wait_for(std::chrono::seconds(0)) != std::future_status::ready) and (it != system_plugins_list.end())) {
		cout << "Setting rate in system " << static_cast<int>((*it)->system->get_system_id()) << endl;
		(*it)->telemetry->set_rate_position_async(1.0, [it, &prom, &sys_rate_correct, expected_systems](Telemetry::Result result) {
			if (result != Telemetry::Result::Success) {
				cerr << "Failure to set rate in system " << static_cast<int>((*it)->system->get_system_id()) << endl;
				prom.set_value(result);
			} else {
				cout << "Correctly set rate in system " << static_cast<int>((*it)->system->get_system_id()) << endl;
				++sys_rate_correct;

				if (sys_rate_correct == expected_systems) {
					prom.set_value(result);
				}
			}
		});
		std::advance(it, 1);
	}

	cout << "Waiting for the result of the rate setting operation" << endl;
	if (fut.get() != Telemetry::Result::Success) {
		exit(static_cast<int>(ProRetCod::TELEMETRY_FAILURE));
	} else {
		cout << "All rates defined" <<endl;
	}

	return static_cast<int>(ProRetCod::OK);
}
