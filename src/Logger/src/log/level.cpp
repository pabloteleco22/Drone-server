#include "level.hpp"

using simple_logger::Level;

Level::Level(const Level *other) {
    level_number = other->level_number;
    color = other->color;
    level_name = other->level_name;
}

Level::~Level() {}

bool Level::operator>=(const Level &other) const {
    return level_number >= other.level_number;
}

bool Level::operator>(const Level &other) const {
    return level_number > other.level_number;
}

bool Level::operator<=(const Level &other) const {

    return level_number <= other.level_number;
}

bool Level::operator<(const Level &other) const {

    return level_number < other.level_number;
}

bool Level::operator==(const Level &other) const {

    return level_number == other.level_number;
}

bool Level::operator!=(const Level &other) const {

    return level_number != other.level_number;
}


string Level::get_color() const {
    return color;
}

string Level::get_level_name() const {
    return level_name;
}