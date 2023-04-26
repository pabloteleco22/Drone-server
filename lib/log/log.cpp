#include "log.hpp"
#include <iomanip>
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;

using namespace simple_logger;

/** Level **/
Level::Level(const Level &other) {
    level_number = other.level_number;
    color = other.color;
    level_name = other.level_name;
}

Level::Level(const Level *other) {
    level_number = other->level_number;
    color = other->color;
    level_name = other->level_name;
}

Level::Level(std::shared_ptr<const Level> other) {
    level_number = other->level_number;
    color = other->level_number;
    level_name = other->level_name;
};

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
WriterLogger::WriterLogger(shared_ptr<const LoggerDecoration> decoration) : Logger() {
    this->decoration = decoration;
    level_filter = shared_ptr<const LevelFilter>(new DefaultFilter);
}

WriterLogger::WriterLogger(const WriterLogger &other) : Logger(other) {
    this->decoration = other.decoration;
    level_filter = other.level_filter;
}

void WriterLogger::set_level_filter(shared_ptr<const LevelFilter> level_filter) {
    this->level_filter = level_filter;
}

/** StreamLogger **/
StreamLogger::StreamLogger(const StreamLogger &other) : WriterLogger(other) {
    this->stream = other.stream;
    this->decoration = other.decoration;
}

StreamLogger::StreamLogger(shared_ptr<std::ostream> stream, const string &greeting_message) :
                WriterLogger(shared_ptr<const LoggerDecoration>{new VoidLoggerDecoration}) {
    this->stream = stream;

    DefaultGreeter greeter;
    greetings(greeter.greetings(greeting_message));
}

StreamLogger::StreamLogger(shared_ptr<std::ostream> stream, shared_ptr<const LoggerDecoration> decoration, const string &greeting_message) :
                WriterLogger(decoration) {
    this->stream = stream;

    DefaultGreeter greeter;
    greetings(greeter.greetings(greeting_message));
}

StreamLogger::StreamLogger(shared_ptr<std::ostream> stream, shared_ptr<const Greeter> greeter, const string &greeting_message) :
                WriterLogger(shared_ptr<const LoggerDecoration>{new VoidLoggerDecoration}) {
    this->stream = stream;

    greetings(greeter->greetings(greeting_message));
}

StreamLogger::StreamLogger(shared_ptr<std::ostream> stream, shared_ptr<const LoggerDecoration> decoration, shared_ptr<const Greeter> greeter, const string &greeting_message) :
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

void StreamLogger::write(shared_ptr<const Level> level, const string &message) {
    write(*level, message);
}

void StreamLogger::greetings(const string &g) const {
    (*stream) << g << endl;
}

const string StreamLogger::default_greeting_message{"Starting stream logger"};

/** StandardLogger **/
StandardLogger::StandardLogger(const string &greeting_message) :
            StreamLogger(shared_ptr<std::ostream>{&cout, [](std::ostream *) {}}, shared_ptr<const Greeter>{new ColorfulDefaultGreeter}, greeting_message) {}

StandardLogger::StandardLogger(const StandardLogger &other) : StreamLogger(other) {}

StandardLogger::StandardLogger(shared_ptr<const LoggerDecoration> decoration, const string &greeting_message) :
            StreamLogger(shared_ptr<std::ostream>{&cout, [](std::ostream *) {}}, decoration, shared_ptr<const Greeter>{new ColorfulDefaultGreeter}, greeting_message) {}

StandardLogger::StandardLogger(shared_ptr<const Greeter> greeter, const string &greeting_message) :
            StreamLogger(shared_ptr<std::ostream>{&cout, [](std::ostream *) {}}, shared_ptr<const LoggerDecoration>{new VoidLoggerDecoration}, greeter, greeting_message) {}

StandardLogger::StandardLogger(shared_ptr<const LoggerDecoration> decoration, shared_ptr<const Greeter> greeter, const string &greeting_message) :
            StreamLogger(shared_ptr<std::ostream>{&cout, [](std::ostream *) {}}, decoration, greeter, greeting_message) {}

void StandardLogger::write(const Level &level, const string &message) {
    if (level_filter->filter(level)) {
        (*stream) << level.get_color() << "["
            << decoration->get_decoration()
            << level.get_level_name() << "]\033[0m " << message << endl;
    }
}

void StandardLogger::write(shared_ptr<const Level> level, const string &message) {
    write(*level, message);
}

const string StandardLogger::default_greeting_message{"Starting standard logger"};

/** ThreadLogger **/
ThreadLogger::ThreadLogger(shared_ptr<Logger> other) {
    logger = other;
}

void ThreadLogger::write(const Level &level, const string &message) {
    mut.lock();

    logger->write(level, message);

    mut.unlock();
}

void ThreadLogger::write(shared_ptr<const Level> level, const string &message) {
    write(*level, message);
}

void ThreadLogger::set_level_filter(shared_ptr<const LevelFilter> level_filter) {
    logger->set_level_filter(level_filter);
}

/** BiLogger **/
BiLogger::BiLogger(shared_ptr<Logger> logger1, shared_ptr<Logger> logger2) {
    this->logger1 = logger1;
    this->logger2 = logger2;
}

void BiLogger::write(const Level &level, const string &message) {
    logger1->write(level, message);
    logger2->write(level, message);
}

void BiLogger::write(shared_ptr<const Level> level, const string &message) {
    write(*level, message);
}

void BiLogger::set_level_filter(shared_ptr<const LevelFilter> level_filter) {
    logger1->set_level_filter(level_filter);
    logger2->set_level_filter(level_filter);
}