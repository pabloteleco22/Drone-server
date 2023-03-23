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

int RandomFlag::MaxMin::get_max() {
    return this->max;
}

int RandomFlag::MaxMin::get_min() {
    return this->min;
}

int RandomFlag::MaxMin::get_interval() {
    return this->max - this->min;
}

RandomFlag::RandomFlag(MaxMin north_m, MaxMin east_m, MaxMin down_m) {
    srand(time(NULL));

    float n;
    float e;
    float d;

    if (north_m.get_interval() != 0)
        n = (float)(north_m.get_min() + rand() % north_m.get_interval()) + (float)(rand()) / (float)(RAND_MAX);
    else
        n = north_m.get_max();

    if (east_m.get_interval() != 0)
        e = (float)(east_m.get_min() + rand() % east_m.get_interval()) + (float)(rand()) / (float)(RAND_MAX);
    else
        e = east_m.get_max();

    if (down_m.get_interval() != 0)
        d = (float)(down_m.get_min() + rand() % down_m.get_interval()) + (float)(rand()) / (float)(RAND_MAX);
    else
        d = down_m.get_max();

    this->pos = new Telemetry::PositionNed{n, e, d};
}

RandomFlag::~RandomFlag() {
    delete this->pos;
}

Telemetry::PositionNed RandomFlag::get_flag_position() const {
    return *(this->pos);
}

RandomFlag::operator std::string() const {
    return ("North [m]: " + std::to_string(this->pos->north_m) + "\n"
            + "East [m]: " + std::to_string(this->pos->east_m) + "\n"
            + "Down [m]: " + std::to_string(this->pos->down_m));
}

RandomFlag::operator Telemetry::PositionNed() const {
    return *(this->pos);
}

Telemetry::PositionNed FixedFlag::get_flag_position() const {
    return this->pos;
}

FixedFlag::operator std::string() const {
    return ("North [m]: " + std::to_string(this->pos.north_m) + "\n"
            + "East [m]: " + std::to_string(this->pos.east_m) + "\n"
            + "Down [m]: " + std::to_string(this->pos.down_m));
}

FixedFlag::operator Telemetry::PositionNed() const {
    return this->pos;
}