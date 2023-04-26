#include "../lib/log/log.hpp"

#include <iostream>
#include <fstream>

using std::cout;
using std::endl;
using std::shared_ptr;

using namespace simple_logger;

int main() {
    /*
    shared_ptr<const Level> level{shared_ptr<Level>(new Level)};
    shared_ptr<const Level> debug{shared_ptr<Level>(new Debug)};
    shared_ptr<const Level> info{shared_ptr<Level>(new Info)};
    shared_ptr<const Level> warning{shared_ptr<Level>(new Warning)};
    shared_ptr<const Level> error{shared_ptr<Level>(new Error)};
    */
    Debug debug;
    Info info;
    Warning warning;
    Error error;

    shared_ptr<UserCustomFilter> user_filter{new UserCustomFilter{
        [&warning](const Level &level) {
            return level == warning;
        }
    }};

    cout << "Standard logger" << endl;
    shared_ptr<Logger> standard_logger{new StandardLogger};
    cout << "All levels" << endl;
    standard_logger->write(debug, "Debug message");
    standard_logger->write(info, "Info message");
    standard_logger->write(warning, "Warning message");
    standard_logger->write(error, "Error message");

    cout << endl << "Custom filter" << endl;
    standard_logger->set_level_filter(user_filter);

    standard_logger->write(debug, "Debug message");
    standard_logger->write(info, "Info message");
    standard_logger->write(warning, "Warning message");
    standard_logger->write(error, "Error message");

    cout << endl << "Standard stream logger" << endl;
    shared_ptr<Logger> standard_stream_logger{new StreamLogger{shared_ptr<std::ostream>(&cout, [](void *) {})}};
    cout << "All levels" << endl;
    standard_stream_logger->write(debug, "Debug message");
    standard_stream_logger->write(info, "Info message");
    standard_stream_logger->write(warning, "Warning message");
    standard_stream_logger->write(error, "Error message");

    cout << endl << "Custom filter" << endl;
    standard_stream_logger->set_level_filter(user_filter);

    standard_stream_logger->write(debug, "Debug message");
    standard_stream_logger->write(info, "Info message");
    standard_stream_logger->write(warning, "Warning message");
    standard_stream_logger->write(error, "Error message");

    shared_ptr<Logger> stream_logger{new StreamLogger{shared_ptr<std::ostream>{new std::ofstream{"logs/test/log_demo.log", std::ios::out}}}};
    stream_logger->write(info, "All levels");
    stream_logger->write(debug, "Debug message");
    stream_logger->write(info, "Info message");
    stream_logger->write(warning, "Warning message");
    stream_logger->write(error, "Error message");

    stream_logger->write(info, "Custom filter");
    stream_logger->set_level_filter(user_filter);

    stream_logger->write(debug, "Debug message");
    stream_logger->write(info, "Info message");
    stream_logger->write(warning, "Warning message");
    stream_logger->write(error, "Error message");

    cout << endl << "Thread standard logger" << endl;
    shared_ptr<Logger> thread_standard_logger{new ThreadLogger{shared_ptr<Logger>{new StandardLogger{shared_ptr<LoggerDecoration>{new TimedLoggerDecoration}}}}};

    cout << "All levels" << endl;
    thread_standard_logger->write(debug, "Debug message");
    thread_standard_logger->write(info, "Info message");
    thread_standard_logger->write(warning, "Warning message");
    thread_standard_logger->write(error, "Error message");

    cout << endl << "Custom filter" << endl;
    thread_standard_logger->set_level_filter(user_filter);

    thread_standard_logger->write(debug, "Debug message");
    thread_standard_logger->write(info, "Info message");
    thread_standard_logger->write(warning, "Warning message");
    thread_standard_logger->write(error, "Error message");

    shared_ptr<Logger> thread_stream_logger{new ThreadLogger{shared_ptr<Logger>{new StreamLogger{
                                                        shared_ptr<std::ostream>{new std::ofstream{"logs/test/thread_log_demo.log", std::ios::out}},
                                                        shared_ptr<LoggerDecoration>{new TimedLoggerDecoration}}}}};

    thread_stream_logger->write(info, "All levels");
    thread_stream_logger->write(debug, "Debug message");
    thread_stream_logger->write(info, "Info message");
    thread_stream_logger->write(warning, "Warning message");
    thread_stream_logger->write(error, "Error message");

    thread_stream_logger->write(info, "Custom filter");
    thread_stream_logger->set_level_filter(user_filter);

    thread_stream_logger->write(debug, "Debug message");
    thread_stream_logger->write(info, "Info message");
    thread_stream_logger->write(warning, "Warning message");
    thread_stream_logger->write(error, "Error message");

    shared_ptr<UserCustomGreeter> custom_greeter{new UserCustomGreeter{[](const string &m) {
        HourLoggerDecoration decoration;

        return "\033[1;104m[" + decoration.get_decoration() + "Greetings]\033[0m " + m;
    }}};

    cout << endl << "Bi logger" << endl;
    shared_ptr<LoggerDecoration> logger_decoration{new HourLoggerDecoration};
    shared_ptr<Logger> bi_logger{new BiLogger{shared_ptr<Logger>{new ThreadLogger{shared_ptr<Logger>{new StandardLogger{logger_decoration, custom_greeter, "Starting thread standard logger"}}}},
                                                        shared_ptr<Logger>{new ThreadLogger{shared_ptr<Logger>{new StreamLogger{shared_ptr<std::ofstream>{new std::ofstream{"logs/test/bilog_demo.log", std::ios::out}}, logger_decoration}}}}}};

    bi_logger->write(info, "All levels");
    bi_logger->write(debug, "Debug message");
    bi_logger->write(info, "Info message");
    bi_logger->write(warning, "Warning message");
    bi_logger->write(error, "Error message");

    bi_logger->write(info, "Custom filter");
    bi_logger->set_level_filter(user_filter);

    bi_logger->write(debug, "Debug message");
    bi_logger->write(info, "Info message");
    bi_logger->write(warning, "Warning message");
    bi_logger->write(error, "Error message");

    shared_ptr<Logger> logger{new StandardLogger};

    return 0;
}