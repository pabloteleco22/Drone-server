#include "log.hpp"
#include <iostream>

using std::cout;
using std::endl;

/** Level **/
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

bool Level::operator>(const Level &other) const {
    return level_number > other.level_number;
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

/** Logger **/
Logger::Logger() {
    min_level = std::make_shared<Level>(new Level);
}

Logger::Logger(Logger *other) {
    min_level = other->min_level;
    stream = other->stream;
}

Logger::Logger(shared_ptr<Logger> other) {
    min_level = other->min_level;
    stream = other->stream;
}

void Logger::set_min_level(std::shared_ptr<Level> level) {
    min_level = level;
}

/** TimedLogger **/
TimedLogger::TimedLogger() : Logger() {
    start_time = std::chrono::steady_clock::now();
}

TimedLogger::TimedLogger(TimedLogger *other) : Logger(other) {
    start_time = other->start_time;
}

TimedLogger::TimedLogger(shared_ptr<TimedLogger> other) : Logger(other) {
    start_time = other->start_time;
}

double TimedLogger::get_timestamp() const {
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start_time).count();
}

/** StreamLogger **/
StreamLogger::StreamLogger(shared_ptr<std::ostream> stream) : TimedLogger() {
    this->stream = stream;
}

void StreamLogger::write(std::shared_ptr<Level> level, const string &message) {
    if ((*level >= *min_level) and (level->is_printable())) {
        (*stream) << "["
            << get_timestamp()
            << " | " << level->get_level_name() << "] " << message << endl;
    }
}

/** StandardLogger **/
StandardLogger::StandardLogger() : StreamLogger(std::shared_ptr<std::ostream>(&cout, [](void *) {})) {}

StandardLogger::StandardLogger(StreamLogger *other) : StreamLogger(other) {
    stream = std::shared_ptr<std::ostream>(&cout, [](void *) {});
}

StandardLogger::StandardLogger(shared_ptr<StreamLogger> other) : StreamLogger(other) {
    stream = std::shared_ptr<std::ostream>(&cout, [](void *) {});
}

void StandardLogger::write(std::shared_ptr<Level> level, const string &message) {
    if ((*level >= *min_level) and (level->is_printable())) {

        (*stream) << level->get_color() << "["
            << get_timestamp()
            << " | " << level->get_level_name() << "]\033[0m " << message << endl;
    }
}

/** BiLogger **/
BiLogger::BiLogger(shared_ptr<std::ostream> stream) : TimedLogger() {
    this->stream = stream;
}

void BiLogger::write(std::shared_ptr<Level> level, const string &message) {
    std_logger.write(level, message);
    stream_logger.write(level, message);
}

void BiLogger::set_min_level(shared_ptr<Level> level) {
    std_logger.set_min_level(level);
    stream_logger.set_min_level(level);
}

/** ThreadStandardLogger **/
void ThreadStandardLogger::write(std::shared_ptr<Level> level, const string &message) {
    mut.lock();

    StandardLogger::write(level, message);

    mut.unlock();
}

void ThreadStandardLogger::set_min_level(shared_ptr<Level> level) {
    mut.lock();

    Logger::set_min_level(level);

    mut.unlock();
}