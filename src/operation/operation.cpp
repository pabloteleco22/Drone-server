/** Copyright (C) 2023  Pablo López Sedeño
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
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