#include "../poly/polygon.hpp"
#include "../../../src/missionhelper/missionhelper.hpp"
#include <mutex>

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