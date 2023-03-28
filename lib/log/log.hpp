#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <ostream>
#include <mutex>

using std::string;
using std::shared_ptr;

struct Level {
    Level() {};
    Level(Level *other);
    Level(shared_ptr<Level> other);
    bool operator>=(const Level &other) const;
    bool operator>(const Level &other) const;
    string get_color() const;
    string get_level_name() const;
    bool is_printable() const;

    protected:
        unsigned short level_number{0};
        string color{"\033[0m"};
        string level_name{"Level"};
        bool printable{false};
};

struct Debug : public Level {
    using Level::Level;
    using Level::operator>=;
    using Level::operator>;
    Debug() {
        level_number = 51;
        color = "\033[1;32m";
        level_name = "Debug";
        printable = true;
    }
};

struct Info : public Level {
    using Level::Level;
    using Level::operator>=;
    using Level::operator>;
    Info() {
        level_number = 102;
        color = "\033[1;34m";
        level_name = "Info";
        printable = true;
    }
};

struct Warning : public Level {
    using Level::Level;
    using Level::operator>=;
    using Level::operator>;
    Warning() {
        level_number = 153;
        color = "\033[1;33m";
        level_name = "Warning";
        printable = true;
    }
};

struct Error : public Level {
    using Level::Level;
    using Level::operator>=;
    using Level::operator>;
    Error() {
        level_number = 204;
        color = "\033[1;31m";
        level_name = "Error";
        printable = true;
    }
};

struct Silence : public Level {
    using Level::Level;
    using Level::operator>=;
    using Level::operator>;
    Silence() {
        level_number = 255;
        level_name = "Silence";
    }
};

struct Logger {
    Logger();
    Logger(Logger *other);
    Logger(shared_ptr<Logger> other);
    virtual void write(shared_ptr<Level> level, const string &message) = 0;
    virtual void set_min_level(shared_ptr<Level> level);

    protected:
        shared_ptr<Level> min_level;
        shared_ptr<std::ostream> stream;
};

class TimedLogger : public Logger {
    std::chrono::time_point<std::chrono::steady_clock> start_time;

    protected:
        virtual double get_timestamp() const;

    public:
        TimedLogger();
        TimedLogger(TimedLogger *other);
        TimedLogger(shared_ptr<TimedLogger> other);
};

struct StandardLogger : public TimedLogger {
    StandardLogger();
    StandardLogger(TimedLogger *other);
    StandardLogger(shared_ptr<TimedLogger> other);
    virtual void write(shared_ptr<Level> level, const string &message) override;
};

struct StreamLogger : public TimedLogger {
    StreamLogger(shared_ptr<std::ostream> stream);
    StreamLogger(TimedLogger *other) : TimedLogger(other) {};
    StreamLogger(shared_ptr<TimedLogger> other) : TimedLogger(other) {};
    virtual void write(shared_ptr<Level> level, const string &message) override;
};

struct BiLogger : public TimedLogger {
    BiLogger(shared_ptr<std::ostream> stream);
    BiLogger(TimedLogger *other) : TimedLogger(other) {};
    BiLogger(shared_ptr<TimedLogger> other) : TimedLogger(other) {};
    virtual void write(shared_ptr<Level> level, const string &message) override;
    virtual void set_min_level(shared_ptr<Level> level) override;

    protected:
        StandardLogger std_logger;
        StreamLogger stream_logger{stream};
};

struct ThreadStandardLogger : public StandardLogger {
    ThreadStandardLogger() : StandardLogger() {};
    ThreadStandardLogger(TimedLogger *other) : StandardLogger(other) {};
    ThreadStandardLogger(shared_ptr<TimedLogger> other) : StandardLogger(other) {};
    void write(shared_ptr<Level> level, const string &message) override;
    void set_min_level(shared_ptr<Level> level) override;

    private:
        std::mutex mut;
};