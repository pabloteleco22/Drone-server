#include <string>
#include <barrier>
#include <mutex>
#include <functional>

using std::string;
using std::mutex;
using std::function;
using std::barrier;

class ProRetCod {
	int code;
	string message;

	protected:
		ProRetCod(int code, string message) : code{code}, message{message} {}

	public:
		ProRetCod(const ProRetCod &other) : code{other.code}, message{other.message} {}
		ProRetCod &operator=(const ProRetCod &other) {
			code = other.code;
			message = other.message;

			return *this;
		}
		bool operator==(const ProRetCod &other) {
			return code == other.code;
		}
		bool operator!=(const ProRetCod &other) {
			return code != other.code;
		}

		int get_code() const {
			return code;
		}
		string get_string() const {
			return message;
		}
};

struct OkCode : public ProRetCod {
	OkCode() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct BadArgument : public ProRetCod {
	BadArgument() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct ConnectionFailed : public ProRetCod {
	ConnectionFailed() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct NoSystemsFound : public ProRetCod {
	NoSystemsFound() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct TelemetryFailure : public ProRetCod {
	TelemetryFailure() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct ActionFailure : public ProRetCod {
	ActionFailure() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct OffboardFailure : public ProRetCod {
	OffboardFailure() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct MissionFailure : public ProRetCod {
	MissionFailure() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct UnknownFailure : public ProRetCod {
	UnknownFailure() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

class OperationTools {
	OkCode ok_code;
	ProRetCod operation_code{ok_code};
	string operation_name;
	mutable mutex mut;
	bool critical{false};

	public:
		OperationTools();
		ProRetCod get_status_code() const;
		bool is_critical() const;
		string get_name() const;
		void set_failure(const ProRetCod error_code, bool critical=false);
		void set_name(const string operation_name);
};

class Operation {
    OperationTools *operation_tools;
	barrier<function<void()>> *sync_point;

    public:
        Operation(OperationTools &operation_tools, barrier<function<void()>> *sync_point);
		ProRetCod new_operation(string operation_name,
							function<ProRetCod(OperationTools &, void *)> operation_action,
							void *operation_args);
};