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
    MCSSuscribePositionFailure () : MissionControllerStatus(code, message) {}

    static const int code;
    static const string message;
};

struct SearchController : public MissionController {
    SearchController(mavsdk::Telemetry *telemetry,
                    const Flag *flag,
                    std::function<void(Flag::Position, bool)> callback,
                    double position_rate, double detection_radius);

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