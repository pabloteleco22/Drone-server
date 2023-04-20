#include "flag.hpp"

#include <stdlib.h>
#include <time.h>

RandomFlag::MaxMin::MaxMin(int n1, int n2) {
    if (n1 > n2) {
        this->max = n1;
        this->min = n2;
    } else {
        this->max = n2;
        this->min = n1;
    }
}

int RandomFlag::MaxMin::get_max() const {
    return this->max;
}

int RandomFlag::MaxMin::get_min() const {
    return this->min;
}

int RandomFlag::MaxMin::get_interval() const {
    return this->max - this->min;
}

RandomFlag::RandomFlag(MaxMin latitude_deg, MaxMin longitude_deg) {
    srand(time(NULL));

    if (latitude_deg.get_interval() != 0)
        pos.latitude_deg = static_cast<float>(latitude_deg.get_min() + rand() % latitude_deg.get_interval()) + static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    else
        pos.latitude_deg = latitude_deg.get_max();

    if (longitude_deg.get_interval() != 0)
        pos.longitude_deg = static_cast<float>(longitude_deg.get_min() + rand() % longitude_deg.get_interval()) + static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    else
        pos.longitude_deg = longitude_deg.get_max();
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

Flag::Position FixedFlag::get_default_pos() {
    return default_pos;
}

FixedFlag::operator std::string() const {
    return ("Latitude [deg]: " + std::to_string(pos.latitude_deg) + "\n"
            + "Longitude [deg]: " + std::to_string(pos.longitude_deg));
}