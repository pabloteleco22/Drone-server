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

struct LoggerDecoration {
    virtual string get_decoration() const = 0;
};

struct VoidLoggerDecoration : public LoggerDecoration {
    virtual string get_decoration() const override;
};

struct TimedLoggerDecoration : public LoggerDecoration {
    TimedLoggerDecoration();
    TimedLoggerDecoration(TimedLoggerDecoration &other);
    virtual string get_decoration() const override;

    private:
        std::chrono::time_point<std::chrono::steady_clock> start_time;
};

struct HourLoggerDecoration : public LoggerDecoration {
    virtual string get_decoration() const override;
};

struct Logger {
    Logger();
    Logger(Logger &other);

    Logger(Logger *other);
    Logger(shared_ptr<Logger> other);

    virtual ~Logger() {};

    virtual void write(shared_ptr<Level> level, const string &message) = 0;
    virtual void set_min_level(shared_ptr<Level> level);

    protected:
        shared_ptr<Level> min_level;
};

struct StreamLogger : public Logger {
    StreamLogger() = delete;

    StreamLogger(shared_ptr<std::ostream> stream);
    StreamLogger(std::ostream *stream);

    StreamLogger(shared_ptr<std::ostream> stream, shared_ptr<LoggerDecoration> decoration);
    StreamLogger(std::ostream *stream, LoggerDecoration *decoration);

    StreamLogger(StreamLogger *other);
    StreamLogger(shared_ptr<StreamLogger> other);

    StreamLogger(Logger *other);
    StreamLogger(shared_ptr<Logger> other);

    virtual void write(shared_ptr<Level> level, const string &message) override;

    protected:
        shared_ptr<std::ostream> stream;
        shared_ptr<LoggerDecoration> decoration;
};

struct StandardLogger : public StreamLogger {
    StandardLogger();

    StandardLogger(LoggerDecoration *decoration);
    StandardLogger(shared_ptr<LoggerDecoration> decoration);

    StandardLogger(StreamLogger *other);
    StandardLogger(shared_ptr<StreamLogger> other);

    virtual void write(shared_ptr<Level> level, const string &message) override;
};

struct ThreadLogger : public Logger {
    ThreadLogger() = delete;

    ThreadLogger(Logger *other);
    ThreadLogger(shared_ptr<Logger> other);

    ThreadLogger(ThreadLogger *other);
    ThreadLogger(shared_ptr<ThreadLogger> other);

    void write(shared_ptr<Level> level, const string &message) override;
    void set_min_level(shared_ptr<Level> level) override;

    private:
        std::mutex mut;
        shared_ptr<Logger> logger;
};

struct BiLogger : public Logger {
    BiLogger(Logger *logger1, Logger *logger2);
    BiLogger(shared_ptr<Logger> logger1, shared_ptr<Logger> logger2);
    BiLogger(BiLogger *logger);
    BiLogger(shared_ptr<BiLogger> logger);

    void write(shared_ptr<Level> level, const string &message) override;
    void set_min_level(shared_ptr<Level> level) override;

    private:
        shared_ptr<Logger> logger1;
        shared_ptr<Logger> logger2;
};
