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

#include "error_control.hpp"

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

/*** Check enough systems ***/
// CheckEnoughSystems //
void CheckEnoughSystems::append_system(float num) {
    mut.lock();
    number_of_systems += num;
    mut.unlock();
}

void CheckEnoughSystems::subtract_system(float num) {
    mut.lock();
    number_of_systems -= num;
    mut.unlock();
}

float CheckEnoughSystems::get_number_of_systems() const {
    mut.lock();
    float n{number_of_systems};
    mut.unlock();

    return n;
}

// PercentageCheck //
PercentageCheck::PercentageCheck(const float expected_systems,
        const float percentage_drones_required) :
        CheckEnoughSystems(expected_systems) {
    this->percentage_required = percentage_drones_required;
    required_systems = expected_systems * percentage_drones_required / 100.0f;
}

bool PercentageCheck::exists_enough_systems() const {
    mut.lock();
    bool exist{number_of_systems >= required_systems};
    mut.unlock();

    return exist;
}

string PercentageCheck::get_status() const {
    mut.lock();
    string status{"Required percentage " + std::to_string(percentage_required) + "%. Systems in use " +
                                                        std::to_string(100.0f * number_of_systems / expected_systems) + "%"};
    mut.unlock();

    return status;
}