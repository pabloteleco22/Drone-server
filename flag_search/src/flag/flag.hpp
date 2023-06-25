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