#include <string>
#include <barrier>
#include <mutex>
#include <functional>

using namespace std;

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

class OperationTools {
	ProRetCod operation_code;
	string operation_name;
	mutable std::mutex mut;
	std::barrier<std::function<void()>> *sync_point;

	public:
		OperationTools(std::barrier<std::function<void()>> *sync_point);
		operator bool() const;
		ProRetCod get_status_code() const;
		string get_name() const;
		void set_failure(const ProRetCod error_code);
		void set_name(const string operation_name);
		void arrive_and_wait();
		void arrive_and_drop();
};

class Operation {
    OperationTools *operation_tools;

    public:
        Operation(OperationTools &operation_tools);
		void new_operation(string operation_name,
							std::function<void(OperationTools &, void *)> operation_action,
							void *operation_args);
};