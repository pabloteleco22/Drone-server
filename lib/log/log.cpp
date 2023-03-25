#include "log.hpp"
#include <iostream>

using std::cout;
using std::endl;

Level::Level() {}

Level::Level(Level *other) {
    level_number = other->level_number;
    color = other->color;
}

Level::Level(std::shared_ptr<Level> other) {
    level_number = other->level_number;
    color = other->level_number;
};

bool Level::operator>=(const Level &other) const {
    return level_number >= other.level_number;
}

string Level::get_color() {
    return color;
}

Logger::Logger() {
    min_level = std::make_shared<Level>(new Level);
}

Logger::Logger(Logger *other) {
    min_level = other->min_level;
}

Logger::Logger(shared_ptr<Logger> other) {
    min_level = other->min_level;
}

void Logger::set_min_level(std::shared_ptr<Level> level) {
    min_level = level;
}

void StandardLogger::write(std::shared_ptr<Level> level, const string &message) {
    if (*level >= *min_level) {
        cout << level->get_color() << message << "\033[0m" << endl;
    }
}