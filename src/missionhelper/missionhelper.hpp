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

#include <mavsdk/plugins/mission/mission.h>

#include <exception>

using namespace mavsdk;

struct CannotMakeMission : public std::exception {
    CannotMakeMission(std::string message);
    const char *what() const noexcept override;

    private:
        std::string message;
};


struct MissionHelper {
    virtual ~MissionHelper() {}

    /**
     * @brief Builds a mission for a system with a given identifier and a number of systems that will also participateBuilds a mission 
    */
    virtual void new_mission(const unsigned int number_of_systems, std::vector<Mission::MissionItem> &mission, unsigned int system_id=256) const = 0;

    protected:
        /**
         * @brief Fills a MissionItem object 
        */
        static Mission::MissionItem make_mission_item(double latitude_deg, double longitude_deg, float relative_altitude_m,
            float speed_m_s, bool is_fly_through, float gimbal_pitch_deg, float gimbal_yaw_deg,
            Mission::MissionItem::CameraAction camera_action);
};
