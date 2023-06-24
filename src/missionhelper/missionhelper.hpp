#pragma once

#include "../poly/polygon.hpp"
#include <mavsdk/plugins/mission/mission.h>
#include <exception>
#include <mutex>

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

struct PolySplitMission : public MissionHelper {
    PolySplitMission(Polygon area);

    protected:
        Polygon area;

        /**
         * @brief Gets the area corresponding to a given system using a Polygon object
        */
        virtual void get_polygon_of_interest(const unsigned int system_id, const unsigned int number_of_systems, Polygon *polygon_of_interest) const;
};

struct GoCenter : public PolySplitMission {
    using PolySplitMission::PolySplitMission;

    /**
     * @brief Builds a mission for a system with a given identifier and a number of systems that will also participateBuilds a mission 
    */
    void new_mission(const unsigned int number_of_systems, std::vector<Mission::MissionItem> &mission, const unsigned int system_id=256) const override;
};

struct SpiralSweepCenter : public PolySplitMission {
    SpiralSweepCenter(Polygon area, const double separation) : PolySplitMission(area) {
        this->separation = separation;
    };

    /**
     * @brief Builds a mission for a system with a given identifier and a number of systems that will also participateBuilds a mission 
    */
    void new_mission(const unsigned int number_of_systems, std::vector<Mission::MissionItem> &mission, unsigned int system_id=256) const override;

    private:
        double separation;
        static unsigned int auto_system_id;
        static std::mutex mut;
};

struct SpiralSweepEdge : public PolySplitMission {
    SpiralSweepEdge(Polygon area, const double separation) : PolySplitMission(area) {
        this->separation = separation;
    };

    /**
     * @brief Builds a mission for a system with a given identifier and a number of systems that will also participateBuilds a mission 
    */
    void new_mission(const unsigned int number_of_systems, std::vector<Mission::MissionItem> &mission, unsigned int system_id=256) const override;

    private:
        double separation;
        static unsigned int auto_system_id;
        static std::mutex mut;
};

struct ParallelSweep : public PolySplitMission {
    ParallelSweep(Polygon area, const double separation) : PolySplitMission(area) {
        this->separation = separation;
    }

    /**
     * @brief Builds a mission for a system with a given identifier and a number of systems that will also participateBuilds a mission 
    */
    void new_mission(const unsigned int number_of_systems, std::vector<Mission::MissionItem> &mission, unsigned int system_id=256) const override;

    private:
        double separation;
        static unsigned int auto_system_id;
        static std::mutex mut;

        /**
         * @brief Find the points at which a line and a polygon intersect.
        */
        std::vector<Point> cross_point(const Polygon &poly, const Line &l) const;
};