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

#include "../errorcontrol/error_control.hpp"
#include <string>

using std::string;

class MissionControllerStatus : public ProRetCod {
    using ProRetCod::ProRetCod;
};

struct MCSSuccess : public MissionControllerStatus {
    MCSSuccess() : MissionControllerStatus(code, message) {}

    static const int code;
    static const string message;
};

struct MissionController {
    /**
     * @brief Asynchronous method that controls the execution of the mission
    */
    virtual MissionControllerStatus mission_control() = 0;
};