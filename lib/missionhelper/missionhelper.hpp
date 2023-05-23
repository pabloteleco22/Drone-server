#pragma once

#include "../poly/polygon.hpp"
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
    MissionHelper(Polygon area);
    virtual ~MissionHelper() {}
    virtual void new_mission(const unsigned int system_id, const unsigned int number_of_systems, std::vector<Mission::MissionItem> &mission) const = 0;

    protected:
        Polygon area;

        static Mission::MissionItem make_mission_item(double latitude_deg, double longitude_deg, float relative_altitude_m,
            float speed_m_s, bool is_fly_through, float gimbal_pitch_deg, float gimbal_yaw_deg,
            Mission::MissionItem::CameraAction camera_action);

        virtual void get_polygon_of_interest(const unsigned int system_id, const unsigned int number_of_systems, Polygon *polygon_of_interest) const;
};

struct GoCenter : public MissionHelper {
    using MissionHelper::MissionHelper;
    virtual void new_mission(const unsigned int system_id, const unsigned int number_of_systems, std::vector<Mission::MissionItem> &mission) const;
};

struct SpiralSweep : public MissionHelper {
    SpiralSweep(Polygon area, const double separation) : MissionHelper(area) {
        this->separation = separation;
    };
    virtual void new_mission(const unsigned int system_id, const unsigned int number_of_systems, std::vector<Mission::MissionItem> &mission) const;

    private:
        double separation;
};