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

#include <mavsdk/plugins/telemetry/telemetry.h>
#include <string>
#include <array>

#include "../poly/polygon.hpp"

using namespace mavsdk;

struct Flag {
    struct Position {
        double latitude_deg;
        double longitude_deg;

        bool operator==(const Position &other) const {
            return latitude_deg == other.latitude_deg and longitude_deg == other.longitude_deg;
        }
    };

    virtual ~Flag() = 0;
    Position get_flag_position() const;
    operator std::string() const;

    protected:
        Position pos;
};

class FixedFlag : public Flag {
    public:
        FixedFlag();
        FixedFlag(Position pos);
        FixedFlag(const double latitude_deg, const double longitude_deg);

        static constexpr Position default_pos{10.0, 0.0};
};

class RandomFlag : public Flag {
    public:
        class MaxMin {
            private:
                double max;
                double min;
            
            public:
                MaxMin(double n1, double n2);
                MaxMin(const MaxMin &other);
                MaxMin &operator=(const MaxMin &other);
                double get_max() const;
                double get_min() const;
                double get_interval() const;
        };

        inline static const MaxMin default_latitude_deg_interval{10, -10};
        inline static const MaxMin default_longitude_deg_interval{10, -10};

        RandomFlag(const bool use_seed=true);
        RandomFlag(const MaxMin &latitude_deg_interval,
                   const MaxMin &longitude_deg_interval,
                   const bool use_seed=true);

    private:
        MaxMin latitude_deg_interval{default_latitude_deg_interval};
        MaxMin longitude_deg_interval{default_longitude_deg_interval};
};

class RandomFlagPoly : public Flag {
    public:
        RandomFlagPoly(const bool use_seed=true);
        RandomFlagPoly(const Polygon polygon, const bool use_seed=true);

        inline static const std::array default_polygon_vertices{Point{-10, -10},
                                                                Point{-10, 10},
                                                                Point{10, 10},
                                                                Point{10, -10}};
};