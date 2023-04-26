#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <ostream>
#include <mutex>
#include <functional>

using std::string;
using std::shared_ptr;

namespace simple_logger {
/** Levels **/
struct Level {
    Level() {};
    Level(const Level &other);
    Level(const Level *other);
    Level(shared_ptr<const Level> other);
    virtual ~Level() = 0;
    bool operator>=(const Level &other) const;
    bool operator>(const Level &other) const;
    bool operator<=(const Level &other) const;
    bool operator<(const Level &other) const;
    bool operator==(const Level &other) const;
    bool operator!=(const Level &other) const;
    string get_color() const;
    string get_level_name() const;

    protected:
        unsigned short level_number{0};
        string color{"\033[0m"};
        string level_name{"Level"};
};

struct Debug : public Level {
    using Level::Level;
    using Level::operator>=;
    using Level::operator>;
    using Level::operator<=;
    using Level::operator<;
    using Level::operator==;
    using Level::operator!=;
    Debug() {
        level_number = 51;
        color = "\033[1;32m";
        level_name = "Debug";
    }
};

struct Info : public Level {
    using Level::Level;
    using Level::operator>=;
    using Level::operator>;
    using Level::operator<=;
    using Level::operator<;
    using Level::operator==;
    using Level::operator!=;
    Info() {
        level_number = 102;
        color = "\033[1;34m";
        level_name = "Info";
    }
};

struct Warning : public Level {
    using Level::Level;
    using Level::operator>=;
    using Level::operator>;
    using Level::operator<=;
    using Level::operator<;
    using Level::operator==;
    using Level::operator!=;
    Warning() {
        level_number = 153;
        color = "\033[1;33m";
        level_name = "Warn";
    }
};

struct Error : public Level {
    using Level::Level;
    using Level::operator>=;
    using Level::operator>;
    using Level::operator<=;
    using Level::operator<;
    using Level::operator==;
    using Level::operator!=;
    Error() {
        level_number = 204;
        color = "\033[1;31m";
        level_name = "Error";
    }
};

/** Level filters */
struct LevelFilter {
    virtual bool filter(const Level &level) const = 0;
};

struct DefaultFilter : public LevelFilter {
    bool filter(const Level &level) const override;
};

struct UserCustomFilter : public LevelFilter {
    UserCustomFilter(std::function<bool(const Level &level)> custom_filter) :
                                                            custom_filter{custom_filter} {}

    bool filter(const Level &level) const override;

    private:
        std::function<bool(const Level &level)> custom_filter;
};

/** Logger decorations **/
struct LoggerDecoration {
    LoggerDecoration() {};
    LoggerDecoration(LoggerDecoration &) {};
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

/** Greeter **/
struct Greeter {
    virtual string greetings(const string &m) const = 0;
};

struct DefaultGreeter : public Greeter {
    virtual string greetings(const string &m) const override;
};

struct ColorfulDefaultGreeter : public Greeter {
    virtual string greetings(const string &m) const override;
};

struct UserCustomGreeter : public Greeter {
    UserCustomGreeter() = delete;
    UserCustomGreeter(std::function<string(const string &)> custom_greetings);
    virtual string greetings(const string &m) const override;

    private:
        std::function<string(const string &)> custom_greetings;
};

/** Logger **/
struct Logger {
    Logger() {};
    Logger(const Logger &) {};

    virtual ~Logger() {};

    virtual void write(const Level &level, const string &message) = 0;
    virtual void write(shared_ptr<const Level> level, const string &message) = 0;
    virtual void set_level_filter(shared_ptr<const LevelFilter> level_filter) = 0;

    protected:
};

struct WriterLogger : public Logger {
    WriterLogger(const WriterLogger &other);
    WriterLogger(shared_ptr<const LoggerDecoration> decoration);
    virtual void set_level_filter(shared_ptr<const LevelFilter> level_filter) override;

    protected:
        shared_ptr<const LoggerDecoration> decoration;
        shared_ptr<const LevelFilter> level_filter;
};

struct StreamLogger : public WriterLogger {
    StreamLogger() = delete;
    StreamLogger(const StreamLogger &other);
    StreamLogger(shared_ptr<std::ostream> stream, const string &greeting_message=default_greeting_message);
    StreamLogger(shared_ptr<std::ostream> stream, shared_ptr<const LoggerDecoration> decoration, const string &greeting_message=default_greeting_message);
    StreamLogger(shared_ptr<std::ostream> stream, shared_ptr<const Greeter> greeter, const string &greeting_message=default_greeting_message);
    StreamLogger(shared_ptr<std::ostream> stream, shared_ptr<const LoggerDecoration> decoration, shared_ptr<const Greeter> greeter, const string &greeting_message=default_greeting_message);

    virtual void write(const Level &level, const string &message) override;
    virtual void write(shared_ptr<const Level> level, const string &message) override;

    protected:
        shared_ptr<std::ostream> stream;

    private:
        void greetings(const string &g) const;
        static const string default_greeting_message;
};

struct StandardLogger : public StreamLogger {
    StandardLogger(const string &greeting_message=default_greeting_message);
    StandardLogger(const StandardLogger &other);

    StandardLogger(shared_ptr<const LoggerDecoration> decoration, const string &greeting_message=default_greeting_message);
    StandardLogger(shared_ptr<const Greeter> greeter, const string &greeting_message=default_greeting_message);
    StandardLogger(shared_ptr<const LoggerDecoration> decoration, shared_ptr<const Greeter> greeter, const string &greeting_message=default_greeting_message);

    virtual void write(const Level &level, const string &message) override;
    virtual void write(shared_ptr<const Level> level, const string &message) override;

    private:
        static const string default_greeting_message;
};

struct ThreadLogger : public Logger {
    ThreadLogger() = delete;
    ThreadLogger(const ThreadLogger &other) = delete;

    ThreadLogger(shared_ptr<Logger> other);

    void write(const Level &level, const string &message) override;
    void write(shared_ptr<const Level> level, const string &message) override;
    void set_level_filter(shared_ptr<const LevelFilter> level_filter) override;

    private:
        std::mutex mut;
        shared_ptr<Logger> logger;
};

struct BiLogger : public Logger {
    BiLogger() = delete;
    BiLogger(const BiLogger &other) = delete;
    BiLogger(shared_ptr<Logger> logger1, shared_ptr<Logger> logger2);

    void write(const Level &level, const string &message) override;
    void write(shared_ptr<const Level> level, const string &message) override;
    void set_level_filter(shared_ptr<const LevelFilter> level_filter) override;

    private:
        shared_ptr<Logger> logger1;
        shared_ptr<Logger> logger2;
};
};