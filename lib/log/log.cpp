#include "log.hpp"
#include <iostream>

using std::cout;
using std::endl;

Logger::Logger() {
    min_level = new Debug;
}

bool Logger::Level::operator>=(const Level &other) const {
    return level_number >= other.level_number;
}

string Logger::Level::get_color() {
    return color;
}

void Logger::set_min_level(Level *level) {
    min_level = level;
}

void StandardLogger::write(Level *level, const string &message) {
    if (*min_level >= *level) {
        cout << level->get_color() << message << endl;
    }
}