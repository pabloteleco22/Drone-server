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

#include "flag.hpp"

#include <time.h>

Flag::~Flag() {}

Flag::Position Flag::get_flag_position() const {
    return pos;
}

Flag::operator std::string() const {
    return ("Latitude [deg]: " + std::to_string(pos.latitude_deg) + "\n"
        + "Longitude [deg]: " + std::to_string(pos.longitude_deg));
}

FixedFlag::FixedFlag() {
    pos = default_pos;
}

FixedFlag::FixedFlag(Position pos) {
    this->pos = pos;
}

RandomFlag::MaxMin::MaxMin(double n1, double n2) {
    if (n1 > n2) {
        this->max = n1;
        this->min = n2;
    } else {
        this->max = n2;
        this->min = n1;
    }
}

RandomFlag::MaxMin::MaxMin(const MaxMin &other) {
    max = other.max;
    min = other.min;
}

RandomFlag::MaxMin &RandomFlag::MaxMin::operator=(const MaxMin &other) {
    max = other.max;
    min = other.min;

    return *this;
}

double RandomFlag::MaxMin::get_max() const {
    return this->max;
}

double RandomFlag::MaxMin::get_min() const {
    return this->min;
}

double RandomFlag::MaxMin::get_interval() const {
    return this->max - this->min;
}

/**
 * @brief Generates a flag in a given area.
 *
 * @param use_seed:
 * Indicates whether a time seed should be used when generating random numbers.
 * Otherwise the default seed will be used.
 */
RandomFlag::RandomFlag(const bool use_seed) {
    if (use_seed)
        srand(time(NULL));

    latitude_deg_interval = default_latitude_deg_interval;
    longitude_deg_interval = default_longitude_deg_interval;

    if (latitude_deg_interval.get_interval() != 0) {
        pos.latitude_deg = latitude_deg_interval.get_min() + latitude_deg_interval.get_interval() * static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    } else {
        pos.latitude_deg = latitude_deg_interval.get_max();
    }

    if (longitude_deg_interval.get_interval() != 0) {
        pos.longitude_deg = longitude_deg_interval.get_min() + longitude_deg_interval.get_interval() * static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    } else {
        pos.longitude_deg = longitude_deg_interval.get_max();
    }
}

/**
 * @brief Generates a flag in a given area.
 *
 * @param use_seed:
 * Indicates whether a time seed should be used when generating random numbers.
 * Otherwise the default seed will be used.
 */
RandomFlag::RandomFlag(const MaxMin &latitude_deg_interval, const MaxMin &longitude_deg_interval, const bool use_seed) {
    if (use_seed)
        srand(time(NULL));

    this->latitude_deg_interval = latitude_deg_interval;
    this->longitude_deg_interval = longitude_deg_interval;

    if (latitude_deg_interval.get_interval() != 0) {
        pos.latitude_deg = latitude_deg_interval.get_min() + latitude_deg_interval.get_interval() * static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    } else {
        pos.latitude_deg = latitude_deg_interval.get_max();
    }

    if (longitude_deg_interval.get_interval() != 0) {
        pos.longitude_deg = longitude_deg_interval.get_min() + longitude_deg_interval.get_interval() * static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    } else {
        pos.longitude_deg = longitude_deg_interval.get_max();
    }
}

/**
 * @brief Generates a flag in a given area.
 *
 * @param use_seed:
 * Indicates whether a time seed should be used when generating random numbers.
 * Otherwise the default seed will be used.
 */
RandomFlagPoly::RandomFlagPoly(const bool use_seed) {
    Polygon polygon;

    for (Point vertex : default_polygon_vertices) {
        polygon.push_back(vertex);
    }

    Point max{ polygon[0] };
    Point min{ polygon[0] };

    size_t poly_size{ polygon.size() };

    for (size_t i = 0; i < poly_size; ++i) {
        if (min.x > polygon[i].x)
            min.x = polygon[i].x;

        if (min.y > polygon[i].y)
            min.y = polygon[i].y;

        if (max.x < polygon[i].x)
            max.x = polygon[i].x;

        if (max.y < polygon[i].y)
            max.y = polygon[i].y;
    }

    RandomFlag::MaxMin maxmin_latitude_deg{ max.x, min.x };
    RandomFlag::MaxMin maxmin_longitude_deg{ max.y, min.y };

    bool cont{ true };
    do {
        Position r_pos{ RandomFlag{maxmin_latitude_deg, maxmin_longitude_deg, use_seed}
            .get_flag_position() };

        if (polygon.is_point_inside(Point{ r_pos.latitude_deg, r_pos.longitude_deg })) {
            pos = r_pos;
            cont = false;
        }
    } while (cont);
}

/**
 * @brief Generates a flag in a given area.
 *
 * @param polygon:
 * Accepts a polygon that delimits the region over which the flag can be generated.
 * Vertices are points that follow the format (latitude, longitude).
 *
 * @param use_seed:
 * Indicates whether a time seed should be used when generating random numbers.
 * Otherwise the default seed will be used.
 */
RandomFlagPoly::RandomFlagPoly(const Polygon polygon, const bool use_seed) {
    Point max{ polygon[0] };
    Point min{ polygon[0] };

    size_t poly_size{ polygon.size() };

    for (size_t i = 0; i < poly_size; ++i) {
        if (min.x > polygon[i].x)
            min.x = polygon[i].x;

        if (min.y > polygon[i].y)
            min.y = polygon[i].y;

        if (max.x < polygon[i].x)
            max.x = polygon[i].x;

        if (max.y < polygon[i].y)
            max.y = polygon[i].y;
    }

    RandomFlag::MaxMin maxmin_latitude_deg{ max.x, min.x };
    RandomFlag::MaxMin maxmin_longitude_deg{ max.y, min.y };

    bool cont{ true };
    do {
        Position r_pos{ RandomFlag{maxmin_latitude_deg, maxmin_longitude_deg, use_seed}
            .get_flag_position() };

        if (polygon.is_point_inside(Point{ r_pos.latitude_deg, r_pos.longitude_deg })) {
            pos = r_pos;
            cont = false;
        }
    } while (cont);
}