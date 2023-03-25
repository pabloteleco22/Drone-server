#include "log.hpp"
#include <iostream>

using std::cout;
using std::endl;

Level::Level() {}

Level::Level(Level *other) {
    level_number = other->level_number;
    color = other->color;
    level_name = other->level_name;
    printable = other->printable;
}

Level::Level(std::shared_ptr<Level> other) {
    level_number = other->level_number;
    color = other->level_number;
    level_name = other->level_name;
    printable = other->printable;
};

bool Level::operator>=(const Level &other) const {
    return level_number >= other.level_number;
}

string Level::get_color() const {
    return color;
}

string Level::get_level_name() const {
    return level_name;
}

bool Level::is_printable() const {
    return printable;
}

Logger::Logger() {
    min_level = std::make_shared<Level>(new Level);
    start_time = std::chrono::steady_clock::now();
}

Logger::Logger(Logger *other) {
    min_level = other->min_level;
    start_time = other->start_time;
}

Logger::Logger(shared_ptr<Logger> other) {
    min_level = other->min_level;
    start_time = other->start_time;
}

void Logger::set_min_level(std::shared_ptr<Level> level) {
    min_level = level;
}

double Logger::get_timestamp() {
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start_time).count();
}

void StandardLogger::write(std::shared_ptr<Level> level, const string &message) {
    if ((*level >= *min_level) and (level->is_printable())) {

        cout << level->get_color() << "["
            << get_timestamp()
            << " | " << level->get_level_name() << "]\033[0m " << message << endl;
    }
}