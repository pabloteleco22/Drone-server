#include "flag.hpp"

#include <time.h>

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

RandomFlag::RandomFlag(const MaxMin &latitude_deg_interval, const MaxMin &longitude_deg_interval) {
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

Flag::Position RandomFlag::get_flag_position() const {
    return pos;
}

RandomFlag::operator std::string() const {
    return ("Latitude [deg]: " + std::to_string(pos.latitude_deg) + "\n"
            + "Longitude [deg]: " + std::to_string(pos.longitude_deg));
}

FixedFlag::FixedFlag() {
    pos = default_pos;
}

FixedFlag::FixedFlag(Position pos) {
    this->pos = pos;
}

Flag::Position FixedFlag::get_flag_position() const {
    return pos;
}

FixedFlag::operator std::string() const {
    return ("Latitude [deg]: " + std::to_string(pos.latitude_deg) + "\n"
            + "Longitude [deg]: " + std::to_string(pos.longitude_deg));
}