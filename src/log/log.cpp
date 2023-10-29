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

#include "log.hpp"
#include <iomanip>
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;

using namespace simple_logger;

/** Level **/
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

/** DefaultFilter **/
bool DefaultFilter::filter(const Level &) const {
    return true;
}

/** UserCustomFilter **/
bool UserCustomFilter::filter(const Level &level) const {
    return custom_filter(level);
}

/** VoidLoggerDecoration **/
string VoidLoggerDecoration::get_decoration() const {
    return "";
}

/** TimedLoggerDecoration **/
TimedLoggerDecoration::TimedLoggerDecoration() : LoggerDecoration() {
    start_time = std::chrono::steady_clock::now();
}

TimedLoggerDecoration::TimedLoggerDecoration(TimedLoggerDecoration &other) : LoggerDecoration() {
    start_time = other.start_time;
}

string TimedLoggerDecoration::get_decoration() const {
    std::ostringstream os;
    os << std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start_time).count() << " | ";
    
    return os.str();
}

/** HourLoggerDecoration **/
string HourLoggerDecoration::get_decoration() const {
    std::ostringstream os;

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::time_t t_now = std::chrono::system_clock::to_time_t(now);
    os << std::put_time(std::localtime(&t_now), "%T") << " | ";

    return os.str();
}

/** DefaultGreeter **/
string DefaultGreeter::greetings(const string &m) const {
    return "[Greetings] " + m;
}

/** ColorfulDefaultGreeter **/
string ColorfulDefaultGreeter::greetings(const string &m) const {
    return "\033[1;104m[Greetings]\033[0m " + m;
}

/** UserCustomGreeter **/
UserCustomGreeter::UserCustomGreeter(std::function<string(const string &)> custom_greetings) {
    this->custom_greetings = custom_greetings;
}

string UserCustomGreeter::greetings(const string &m) const {
    return custom_greetings(m);
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

void StreamLogger::write(const Level &level, const string &message) {
    if (level_filter->filter(level)) {
        (*stream) << "["
            << decoration->get_decoration()
            << level.get_level_name() << "] " << message << endl;
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

void StandardLogger::write(const Level &level, const string &message) {
    if (level_filter->filter(level)) {
        (*stream) << level.get_color() << "["
            << decoration->get_decoration()
            << level.get_level_name() << "]\033[0m " << message << endl;
    }
}

const string StandardLogger::default_greeting_message{"Starting standard logger"};

/** ThreadLogger **/
ThreadLogger::ThreadLogger(Logger *other) {
    logger = other;
}

void ThreadLogger::write(const Level &level, const string &message) {
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

void BiLogger::write(const Level &level, const string &message) {
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

/** StandardLoggerBuilder **/
Logger *StandardLoggerBuilder::build() {
    return new StandardLogger{decoration, greeter, greeting_string};
}

StandardLoggerBuilder &StandardLoggerBuilder::set_decoration(const LoggerDecoration *decoration) {
    this->decoration = decoration;

    return *this;
}

StandardLoggerBuilder &StandardLoggerBuilder::set_greeter(const Greeter *greeter) {
    this->greeter = greeter;

    return *this;
}

StandardLoggerBuilder &StandardLoggerBuilder::set_greeting_string(const string &greeting_string) {
    this->greeting_string = greeting_string;

    return *this;
}

StandardLoggerBuilder &StandardLoggerBuilder::reset_config() {
    greeting_string = StandardLogger::default_greeting_message;
    greeter = &default_greeter;
    decoration = &default_decoration;

    return *this;
}

StreamLoggerBuilder::StreamLoggerBuilder(std::ostream *stream) {
    this->stream = stream;
}

Logger *StreamLoggerBuilder::build() {
    return new StreamLogger{stream, decoration, greeter, greeting_string};
}

StreamLoggerBuilder &StreamLoggerBuilder::set_stream(std::ostream *stream) {
    this->stream = stream;

    return *this;
}

StreamLoggerBuilder &StreamLoggerBuilder::set_decoration(const LoggerDecoration *decoration) {
    this->decoration = decoration;

    return *this;
}

StreamLoggerBuilder &StreamLoggerBuilder::set_greeter(const Greeter *greeter) {
    this->greeter = greeter;

    return *this;
}

StreamLoggerBuilder &StreamLoggerBuilder::set_greeting_string(const string &greeting_string) {
    this->greeting_string = greeting_string;

    return *this;
};

StreamLoggerBuilder &StreamLoggerBuilder::reset_config() {
    greeting_string = StreamLogger::default_greeting_message;
    greeter = &default_greeter;
    decoration = &default_decoration;

    return *this;
}