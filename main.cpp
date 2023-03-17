#include <mavsdk/mavsdk.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace mavsdk;
using std::cout;
using std::cerr;
using std::endl;

// Process return code
enum class ProRetCod : int {
	OK = 0,
	BAD_ARGUMENT = 1,
	CONNECTION_FAILED = 2,
	NO_SYSTEMS_FOUND = 3
};

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cerr << "Use: " << "<port1> <port2> ..." << endl;

		exit(static_cast<int>(ProRetCod::BAD_ARGUMENT));
	}

	// Constants
	const std::vector<System>::size_type expected_systems {static_cast<unsigned long>(argc - 1)};
	const unsigned int max_attempts {30};

	std::vector<std::string> url_list {};

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

	std::vector<System>::size_type number_of_systems {mavsdk.systems().size()};

	mavsdk.subscribe_on_new_system([&mavsdk, &number_of_systems, &expected_systems]() {
		number_of_systems = mavsdk.systems().size();

		std::shared_ptr<System> s {mavsdk.systems().back()};
		cout << "New system!" << endl;
		cout << "System: " << s->get_system_id() << endl;
		cout << "    Is connected: " << s->is_connected() << endl;
		cout << "    Has autopilot: " << s->has_autopilot() << endl;

		cout << "Systems found: " << number_of_systems << endl;
		cout << "Remaining systems: " << expected_systems - number_of_systems << endl;
		});
	
	unsigned int attempt {0};
	while ((attempt < max_attempts) and (number_of_systems < expected_systems)) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		++attempt;
	}

	if (number_of_systems < expected_systems) {
		cout << "Not all systems found" << endl;
		exit(static_cast<int>(ProRetCod::NO_SYSTEMS_FOUND));
	} else {
		cout << "All systems found" << endl;
	}

	return static_cast<int>(ProRetCod::OK);
}
