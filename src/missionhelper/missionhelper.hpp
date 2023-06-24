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
