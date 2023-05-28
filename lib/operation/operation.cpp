#include "operation.hpp"

/** Operation Status **/
OperationTools::OperationTools(std::barrier<std::function<void(OperationTools &)>> *sync_point) {
    this->operation_name = "";
    this->operation_code = ProRetCod::OK;
    this->sync_point = sync_point;
}

OperationTools::operator bool() const {
    bool ok;

    mut.lock();
    ok = ProRetCod::OK == operation_code;
    mut.unlock();

    return ok;
}

ProRetCod OperationTools::get_status_code() const {
    ProRetCod cod;

    mut.lock();
    cod = operation_code;
    mut.unlock();

    return cod;
}

string OperationTools::get_name() const {
    string name;

    mut.lock();
    name = operation_name;
    mut.unlock();

    return name;
}

void OperationTools::set_failure(const ProRetCod error_code) {
    mut.lock();
    this->operation_code = error_code;
    mut.unlock();
}

void OperationTools::set_name(const string operation_name) {
    mut.lock();
    this->operation_name = operation_name;
    mut.unlock();
}

void OperationTools::arrive_and_wait() {
    sync_point->arrive_and_wait();
}

void OperationTools::arrive_and_drop() {
    sync_point->arrive_and_drop();
}

/** Operation **/
Operation::Operation(OperationTools &operation_tools) {
    this->operation_tools = &operation_tools;
}

void Operation::new_operation(string operation_name,
                                std::function<void(OperationTools &, void *)> operation_action,
                                void *operation_args) {
    operation_tools->set_name(operation_name);
    operation_action(*operation_tools, operation_args);
}