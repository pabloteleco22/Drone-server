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