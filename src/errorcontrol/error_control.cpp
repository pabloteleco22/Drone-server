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