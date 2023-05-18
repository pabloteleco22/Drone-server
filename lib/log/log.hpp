#pragma once

#include <string>
#include <chrono>
#include <ostream>
#include <mutex>
#include <functional>

using std::string;

namespace simple_logger {
/** Levels **/
struct Level {
    Level() {};
    Level(const Level *other);
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
    virtual void set_level_filter(const LevelFilter *level_filter) = 0;

    protected:
};

struct WriterLogger : public Logger {
    WriterLogger(const WriterLogger &other);
    WriterLogger(const LoggerDecoration *decoration);
    virtual void set_level_filter(const LevelFilter *level_filter) override;

    protected:
        const LoggerDecoration *decoration;
        const LevelFilter *level_filter;
};

struct StreamLogger : public WriterLogger {
    StreamLogger() = delete;
    StreamLogger(const StreamLogger &other);
    StreamLogger(std::ostream *stream, const LoggerDecoration *decoration, const Greeter *greeter, const string &greeting_message=default_greeting_message);

    virtual void write(const Level &level, const string &message) override;

    static const string default_greeting_message;

    protected:
        std::ostream *stream;

    private:
        void greetings(const string &g) const;
};

struct StandardLogger : public StreamLogger {
    StandardLogger(const StandardLogger &other);

    StandardLogger(const LoggerDecoration *decoration, const Greeter *greeter, const string &greeting_message=default_greeting_message);

    virtual void write(const Level &level, const string &message) override;

    static const string default_greeting_message;
};

struct ThreadLogger : public Logger {
    ThreadLogger() = delete;
    ThreadLogger(const ThreadLogger &other) = delete;

    ThreadLogger(Logger *other);

    void write(const Level &level, const string &message) override;
    void set_level_filter(const LevelFilter *level_filter) override;
    void set_logger(Logger *logger);

    private:
        std::mutex mut;
        Logger *logger;
};

struct BiLogger : public Logger {
    BiLogger() = delete;
    BiLogger(const BiLogger &other) = delete;
    BiLogger(Logger *logger1, Logger *logger2);

    void write(const Level &level, const string &message) override;
    void set_level_filter(const LevelFilter *level_filter) override;
    void set_first_logger(Logger *logger);
    void set_second_logger(Logger *logger);

    private:
        Logger *logger1;
        Logger *logger2;
};

/** LoggerBuilder **/
struct LoggerBuilder {
    virtual Logger *build() = 0;
};

class StandardLoggerBuilder : public LoggerBuilder {
    VoidLoggerDecoration default_decoration;
    ColorfulDefaultGreeter default_greeter;
    string greeting_string{StandardLogger::default_greeting_message};

    const LoggerDecoration *decoration{&default_decoration};
    const Greeter *greeter{&default_greeter};

    public:
        Logger *build() override;
        StandardLoggerBuilder &set_decoration(const LoggerDecoration *decoration);
        StandardLoggerBuilder &set_greeter(const Greeter *greeter);
        StandardLoggerBuilder &set_greeting_string(const string &greeting_string);
        StandardLoggerBuilder &reset_config();
};

class StreamLoggerBuilder : public LoggerBuilder {
    VoidLoggerDecoration default_decoration;
    DefaultGreeter default_greeter;
    string greeting_string = StreamLogger::default_greeting_message;

    const LoggerDecoration *decoration{&default_decoration};
    const Greeter *greeter{&default_greeter};
    std::ostream *stream;

    public:
        StreamLoggerBuilder(std::ostream *stream);
        Logger *build() override;
        StreamLoggerBuilder &set_stream(std::ostream *stream);
        StreamLoggerBuilder &set_decoration(const LoggerDecoration *decoration);
        StreamLoggerBuilder &set_greeter(const Greeter *greeter);
        StreamLoggerBuilder &set_greeting_string(const string &greeting_string);
        StreamLoggerBuilder &reset_config();
};
};