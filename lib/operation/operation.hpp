#include <string>
#include <barrier>
#include <mutex>
#include <functional>

#include "../errorcontrol/error_control.hpp"

using std::string;
using std::mutex;
using std::function;
using std::barrier;

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

		template<typename T>
		ProRetCod new_operation(string operation_name,
							function<ProRetCod(OperationTools &, T *)> operation_action,
							T *operation_args) {

		operation_tools->set_name(operation_name);

		ProRetCod ret{operation_action(*operation_tools, operation_args)};

		OkCode ok_code;

		if (ret == ok_code) {
			sync_point->arrive_and_wait();
		} else {
			sync_point->arrive_and_drop();
		}

		return ret;
	}
};