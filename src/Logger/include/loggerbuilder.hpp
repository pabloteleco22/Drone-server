#pragma once 

#include "logger.hpp"

namespace simple_logger {
    struct LoggerBuilder {
        virtual Logger *build() = 0;
    };

    class StandardLoggerBuilder : public LoggerBuilder {
        VoidLoggerDecoration default_decoration;
        ColorfulDefaultGreeter default_greeter;
        string greeting_string{ StandardLogger::default_greeting_message };

        const LoggerDecoration *decoration{ &default_decoration };
        const Greeter *greeter{ &default_greeter };

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

        const LoggerDecoration *decoration{ &default_decoration };
        const Greeter *greeter{ &default_greeter };
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