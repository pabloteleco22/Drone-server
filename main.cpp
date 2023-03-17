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
	if (argc != 2) {
		cerr << "Use: " << argv[0] << " udp://:<port>" << endl;

		exit(static_cast<int>(ProRetCod::BAD_ARGUMENT));
	}

	Mavsdk mavsdk;

	cout << "Establishing connection..." << endl;
	ConnectionResult connection_result = mavsdk.add_udp_connection(argv[1]);
	if (connection_result == ConnectionResult::Success) {
		cout << "Connection established" << endl;
	} else {
		cerr << "Connection failed: " << connection_result << endl;

		exit(static_cast<int>(ProRetCod::CONNECTION_FAILED));
	}

	mavsdk.subscribe_on_new_system([&mavsdk]() {
		std::shared_ptr<System> s = mavsdk.systems().back();
		cout << "New system!" << endl;
		cout << "System: " << s->get_system_id() << endl;
		cout << "    Is connected: " << s->is_connected() << endl;
		cout << "    Has autopilot: " << s->has_autopilot() << endl;
		});
	
	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::vector<System>::size_type number_of_systems = mavsdk.systems().size();
	cout << mavsdk.systems().size() << " systems found" << endl;
	if (number_of_systems > 0) {
		cout << mavsdk.systems().size() << " systems found" << endl;
	} else {
		cout << "No systems found" << endl;
		exit(static_cast<int>(ProRetCod::NO_SYSTEMS_FOUND));
	}

	return static_cast<int>(ProRetCod::OK);
}
