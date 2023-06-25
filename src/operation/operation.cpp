/**
 * The MIT License (MIT)
 * Copyright (c) 2023 Pablo López Sedeño
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include "operation.hpp"

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

bool OperationTools::is_critical() const {
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