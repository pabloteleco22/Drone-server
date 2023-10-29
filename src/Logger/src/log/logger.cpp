#include "logger.hpp"

#include <iostream>

using std::cout;
using std::endl;
using namespace simple_logger;

/** LoggerStreamResponse **/
Logger::LoggerStreamResponse::LoggerStreamResponse(const Logger *logger, const Level *level) {
    this->logger = logger;
    this->level = level;
}

void Logger::LoggerStreamResponse::operator<<(std::ostream& (*)(std::ostream&)) {
    flush();
}

void Logger::LoggerStreamResponse::flush() {
    logger->write(*level, message.str());
}

/** Logger **/
Logger::LoggerStreamResponse Logger::operator<<(const Level &level) {
    return LoggerStreamResponse(this, &level);
}

/** WriterLogger **/
WriterLogger::WriterLogger(const LoggerDecoration *decoration) : Logger() {
    this->decoration = decoration;
    level_filter = new DefaultFilter;
}

WriterLogger::WriterLogger(const WriterLogger &other) : Logger(other) {
    this->decoration = other.decoration;
    level_filter = other.level_filter;
}

void WriterLogger::set_level_filter(const LevelFilter *level_filter) {
    this->level_filter = level_filter;
}

/** StreamLogger **/
StreamLogger::StreamLogger(const StreamLogger &other) : WriterLogger(other) {
    this->stream = other.stream;
    this->decoration = other.decoration;
}

StreamLogger::StreamLogger(std::ostream *stream, const LoggerDecoration *decoration, const Greeter *greeter, const string &greeting_message) :
                WriterLogger(decoration) {
    this->stream = stream;

    greetings(greeter->greetings(greeting_message));
}

void StreamLogger::write(const Level &level, const string &message) const {
    if (level_filter->filter(level)) {
        string decoration_str{decoration->get_decoration()};
        
        if (decoration_str == "") {
            (*stream) << "["
                << level.get_level_name() << "] " << message << endl;
        } else {
            (*stream) << "["
                << decoration_str << " | "
                << level.get_level_name() << "] " << message << endl;
        }
    }
}

void StreamLogger::greetings(const string &g) const {
    (*stream) << g << endl;
}

const string StreamLogger::default_greeting_message{"Starting stream logger"};

/** StandardLogger **/
StandardLogger::StandardLogger(const StandardLogger &other) : StreamLogger(other) {}

StandardLogger::StandardLogger(const LoggerDecoration *decoration, const Greeter *greeter, const string &greeting_message) :
            StreamLogger(&cout, decoration, greeter, greeting_message) {}

void StandardLogger::write(const Level &level, const string &message) const {
    if (level_filter->filter(level)) {
        string decoration_str{decoration->get_decoration()};

        
        if (decoration_str == "") {
            (*stream) << level.get_color() << "["
                << level.get_level_name() << "]\033[0m " << message << endl;
        } else {
            (*stream) << level.get_color() << "["
                << decoration->get_decoration() << " | "
                << level.get_level_name() << "]\033[0m " << message << endl;
        }
    }
}

const string StandardLogger::default_greeting_message{"Starting standard logger"};

/** ThreadLogger **/
ThreadLogger::ThreadLogger(Logger *other) {
    logger = other;
}

void ThreadLogger::write(const Level &level, const string &message) const {
    mut.lock();

    logger->write(level, message);

    mut.unlock();
}

void ThreadLogger::set_level_filter(const LevelFilter *level_filter) {
    logger->set_level_filter(level_filter);
}

void ThreadLogger::set_logger(Logger *logger) {
    this->logger = logger;
}

/** BiLogger **/
BiLogger::BiLogger(Logger *logger1, Logger *logger2) {
    this->logger1 = logger1;
    this->logger2 = logger2;
}

void BiLogger::write(const Level &level, const string &message) const {
    logger1->write(level, message);
    logger2->write(level, message);
}

void BiLogger::set_level_filter(const LevelFilter *level_filter) {
    logger1->set_level_filter(level_filter);
    logger2->set_level_filter(level_filter);
}

void BiLogger::set_first_logger(Logger *logger) {
    logger1 = logger;
}

void BiLogger::set_second_logger(Logger *logger) {
    logger2 = logger;
}