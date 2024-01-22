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

#pragma once

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/mission/mission.h>
#include <mavsdk/plugins/action/action.h>

#include "../flag/flag.hpp"
#include "../../../src/missioncontrol/missioncontrol.hpp"
#include <memory>
#include <functional>
#include <mutex>
#include <string>

using std::string;

struct MCSSuscribePositionFailure : public MissionControllerStatus {
    MCSSuscribePositionFailure() : MissionControllerStatus(code, message) {}

    static const int code;
    static const string message;
};

struct SearchController : public MissionController {
    SearchController(mavsdk::Telemetry *telemetry, const Flag *flag, std::function<void(Flag::Position, bool)> callback, double position_rate, double detection_radius);

    /**
     * @brief Asynchronous method that controls the execution of the mission
    */
    MissionControllerStatus mission_control() override;

private:
    mavsdk::Telemetry *telemetry;
    const Flag *flag;
    std::function<void(Flag::Position, bool)> callback;
    double position_rate;
    double detection_radius;
    static std::mutex mut;
    static bool flag_found;
};