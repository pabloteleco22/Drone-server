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

#include "missioncontrol.hpp"

#include <cmath>

/** MissionControllerStatus **/
const int MCSSuscribePositionFailure::code{ 1 };
const string MCSSuscribePositionFailure::message{ "unable to suscribe position" };

/** Search Controller **/
bool SearchController::flag_found{ false };
std::mutex SearchController::mut{};

SearchController::SearchController(mavsdk::Telemetry *telemetry,
    const Flag *flag,
    std::function<void(Flag::Position, bool)> callback,
    double position_rate, double detection_radius) {
    this->telemetry = telemetry;
    this->flag = flag;
    this->callback = callback;
    this->position_rate = position_rate;
    this->detection_radius = detection_radius;
}

MissionControllerStatus SearchController::mission_control() {
    // Establecer a un segundo la frecuencia de transmisión de la posición
    mavsdk::Telemetry::Result result{ telemetry->set_rate_position(position_rate) };
    if (result != mavsdk::Telemetry::Result::Success) {
        MCSSuscribePositionFailure failure;
        return failure;
    }

    // Pedir regularmente la posición de los drones
    telemetry->subscribe_position(
        [this](mavsdk::Telemetry::Position pos) {
            Flag::Position flag_pos{ this->flag->get_flag_position() };
            bool flag_found;
            bool flag_found_by_me{ false };

            if (std::sqrt(
                std::pow(flag_pos.latitude_deg - pos.latitude_deg, 2) +
                std::pow(flag_pos.longitude_deg - pos.longitude_deg, 2)
            ) <= detection_radius) {
                this->mut.lock();
                this->flag_found = true;
                flag_found = true;
                flag_found_by_me = true;
                this->mut.unlock();
            } else {
                this->mut.lock();
                flag_found = this->flag_found;
                this->mut.unlock();
            }

            if (flag_found) {
                this->telemetry->subscribe_position(nullptr);

                this->callback(flag_pos, flag_found_by_me);
            }
        }
    );

    MCSSuccess success;
    return success;
}