#include "loggerbuilder.hpp"

using namespace simple_logger;

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