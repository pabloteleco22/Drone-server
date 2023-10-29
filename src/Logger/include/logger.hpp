#pragma once

#include "level.hpp"
#include "levelfilter.hpp"
#include "decoration.hpp"
#include "greeter.hpp"

#include <sstream>
#include <mutex>

namespace simple_logger {
struct Logger {
    Logger() {};
    Logger(const Logger &) {};

    virtual ~Logger() {};

    virtual void write(const Level &level, const string &message) const = 0;
    virtual void set_level_filter(const LevelFilter *level_filter) = 0;

    class LoggerStreamResponse {
        private:
            const Logger *logger;
            const Level *level;
            std::ostringstream message;
        
        public:
            LoggerStreamResponse(const Logger *logger, const Level *level);
            LoggerStreamResponse &operator<<(auto message) {
                this->message << message;
                
                return *this;
            }
            void operator<<(std::ostream& (*func)(std::ostream&));
            void flush();
    };
    
    LoggerStreamResponse operator<<(const Level &level);
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

    virtual void write(const Level &level, const string &message) const override;

    static const string default_greeting_message;

    protected:
        std::ostream *stream;

    private:
        void greetings(const string &g) const;
};

struct StandardLogger : public StreamLogger {
    StandardLogger(const StandardLogger &other);

    StandardLogger(const LoggerDecoration *decoration, const Greeter *greeter, const string &greeting_message=default_greeting_message);

    virtual void write(const Level &level, const string &message) const override;

    static const string default_greeting_message;
};

struct ThreadLogger : public Logger {
    ThreadLogger() = delete;
    ThreadLogger(const ThreadLogger &other) = delete;

    ThreadLogger(Logger *other);

    void write(const Level &level, const string &message) const override;
    void set_level_filter(const LevelFilter *level_filter) override;
    void set_logger(Logger *logger);

    private:
        mutable std::mutex mut;
        Logger *logger;
};

struct BiLogger : public Logger {
    BiLogger() = delete;
    BiLogger(const BiLogger &other) = delete;
    BiLogger(Logger *logger1, Logger *logger2);

    void write(const Level &level, const string &message) const override;
    void set_level_filter(const LevelFilter *level_filter) override;
    void set_first_logger(Logger *logger);
    void set_second_logger(Logger *logger);

    private:
        Logger *logger1;
        Logger *logger2;
};
};