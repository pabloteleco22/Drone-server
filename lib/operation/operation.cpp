#include "operation.hpp"

/** ProRetCod **/
const int OkCode::code{0};
const string OkCode::message{"OK"};

const int BadArgument::code{1};
const string BadArgument::message{"bad argument"};

const int ConnectionFailed::code{2};
const string ConnectionFailed::message{"connection failed"};

const int NoSystemsFound::code{3};
const string NoSystemsFound::message{"no systems found"};

const int TelemetryFailure::code{4};
const string TelemetryFailure::message{"telemetry failure"};

const int ActionFailure::code{5};
const string ActionFailure::message{"action failure"};

const int OffboardFailure::code{6};
const string OffboardFailure::message{"offboard failure"};

const int MissionFailure::code{7};
const string MissionFailure::message{"mission failure"};

const int UnknownFailure::code{255};
const string UnknownFailure::message{"unknown failure"};

/** Operation Status **/
OperationTools::OperationTools() {
    this->operation_name = "";
}

ProRetCod OperationTools::get_status_code() const {

    mut.lock();
    ProRetCod cod{operation_code};
    mut.unlock();

    return cod;
}

bool OperationTools::get_critical() const {
    bool cr;

    mut.lock();
    cr = critical;
    mut.unlock();

    return cr;
}

string OperationTools::get_name() const {
    string name;

    mut.lock();
    name = operation_name;
    mut.unlock();

    return name;
}

void OperationTools::set_failure(const ProRetCod error_code, bool critical) {
    OkCode ok_code;

    mut.lock();
    this->operation_code = error_code;
    this->critical |= critical;
    mut.unlock();
}

void OperationTools::set_name(const string operation_name) {
    mut.lock();
    if (this->operation_name != operation_name) {
        this->operation_name = operation_name;
        this->operation_code = ok_code;
    }
    mut.unlock();
}

/** Operation **/
Operation::Operation(OperationTools &operation_tools, barrier<function<void()>> *sync_point) {
    this->operation_tools = &operation_tools;
    this->sync_point = sync_point;
}

ProRetCod Operation::new_operation(string operation_name,
                                function<ProRetCod(OperationTools &, void *)> operation_action,
                                void *operation_args) {

    operation_tools->set_name(operation_name);

    ProRetCod ret{operation_action(*operation_tools, operation_args)};

    OkCode ok_code;

    if (operation_tools->get_status_code() == ok_code) {
        sync_point->arrive_and_wait();
    } else {
        sync_point->arrive_and_drop();
    }

    return ret;
}