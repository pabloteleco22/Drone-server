#include "../lib/log/log.hpp"

#include <iostream>
#include <fstream>

using std::cout;
using std::endl;
using std::shared_ptr;

int main() {
    shared_ptr<Level> level{shared_ptr<Level>(new Level)};
    shared_ptr<Level> debug{shared_ptr<Level>(new Debug)};
    shared_ptr<Level> info{shared_ptr<Level>(new Info)};
    shared_ptr<Level> warning{shared_ptr<Level>(new Warning)};
    shared_ptr<Level> error{shared_ptr<Level>(new Error)};
    shared_ptr<Level> silence{shared_ptr<Level>(new Silence)};

    shared_ptr<Logger> standard_logger{shared_ptr<Logger>{new StandardLogger}};

    cout << "Standard logger" << endl;
    cout << "All levels" << endl;
    standard_logger->write(level, "Level message");
    standard_logger->write(debug, "Debug message");
    standard_logger->write(info, "Info message");
    standard_logger->write(warning, "Warning message");
    standard_logger->write(error, "Error message");
    standard_logger->write(silence, "Silence message");

    cout << endl << "Min Warning level" << endl;
    standard_logger->set_min_level(warning);

    standard_logger->write(level, "Level message");
    standard_logger->write(debug, "Debug message");
    standard_logger->write(info, "Info message");
    standard_logger->write(warning, "Warning message");
    standard_logger->write(error, "Error message");
    standard_logger->write(silence, "Silence message");

    shared_ptr<Logger> standard_stream_logger{shared_ptr<Logger>{new StreamLogger{shared_ptr<std::ostream>(&cout, [](void *) {})}}};

    cout << endl << "Standard stream logger" << endl;
    cout << "All levels" << endl;
    standard_stream_logger->write(level, "Level message");
    standard_stream_logger->write(debug, "Debug message");
    standard_stream_logger->write(info, "Info message");
    standard_stream_logger->write(warning, "Warning message");
    standard_stream_logger->write(error, "Error message");
    standard_stream_logger->write(silence, "Silence message");

    cout << endl << "Min Warning level" << endl;
    standard_stream_logger->set_min_level(warning);

    standard_stream_logger->write(level, "Level message");
    standard_stream_logger->write(debug, "Debug message");
    standard_stream_logger->write(info, "Info message");
    standard_stream_logger->write(warning, "Warning message");
    standard_stream_logger->write(error, "Error message");
    standard_stream_logger->write(silence, "Silence message");

    shared_ptr<Logger> stream_logger{shared_ptr<Logger>{new StreamLogger{new std::ofstream{"logs/test/log_demo.log", std::ios::out}}}};

    stream_logger->write(info, "All levels");
    stream_logger->write(level, "Level message");
    stream_logger->write(debug, "Debug message");
    stream_logger->write(info, "Info message");
    stream_logger->write(warning, "Warning message");
    stream_logger->write(error, "Error message");
    stream_logger->write(silence, "Silence message");

    stream_logger->write(info, "Min Warning level");
    stream_logger->set_min_level(warning);

    stream_logger->write(level, "Level message");
    stream_logger->write(debug, "Debug message");
    stream_logger->write(info, "Info message");
    stream_logger->write(warning, "Warning message");
    stream_logger->write(error, "Error message");
    stream_logger->write(silence, "Silence message");

    cout << endl << "Thread standard logger" << endl;
    shared_ptr<Logger> thread_standard_logger{shared_ptr<Logger>{new ThreadLogger{new StandardLogger{new TimedLoggerDecoration}}}};

    cout << "All levels" << endl;
    thread_standard_logger->write(level, "Level message");
    thread_standard_logger->write(debug, "Debug message");
    thread_standard_logger->write(info, "Info message");
    thread_standard_logger->write(warning, "Warning message");
    thread_standard_logger->write(error, "Error message");
    thread_standard_logger->write(silence, "Silence message");

    cout << endl << "Min Warning level" << endl;
    thread_standard_logger->set_min_level(warning);

    thread_standard_logger->write(level, "Level message");
    thread_standard_logger->write(debug, "Debug message");
    thread_standard_logger->write(info, "Info message");
    thread_standard_logger->write(warning, "Warning message");
    thread_standard_logger->write(error, "Error message");
    thread_standard_logger->write(silence, "Silence message");

    shared_ptr<Logger> thread_stream_logger{shared_ptr<Logger>{new ThreadLogger{new StreamLogger{
                                                        new std::ofstream{"logs/test/thread_log_demo.log", std::ios::out}}}}};

    thread_stream_logger->write(info, "All levels");
    thread_stream_logger->write(level, "Level message");
    thread_stream_logger->write(debug, "Debug message");
    thread_stream_logger->write(info, "Info message");
    thread_stream_logger->write(warning, "Warning message");
    thread_stream_logger->write(error, "Error message");
    thread_stream_logger->write(silence, "Silence message");

    thread_stream_logger->write(info, "Min Warning level");
    thread_stream_logger->set_min_level(warning);

    thread_stream_logger->write(level, "Level message");
    thread_stream_logger->write(debug, "Debug message");
    thread_stream_logger->write(info, "Info message");
    thread_stream_logger->write(warning, "Warning message");
    thread_stream_logger->write(error, "Error message");
    thread_stream_logger->write(silence, "Silence message");

    cout << endl << "Bi logger" << endl;
    shared_ptr<LoggerDecoration> logger_decoration{new HourLoggerDecoration};
    shared_ptr<Logger> bi_logger{shared_ptr<Logger>{new BiLogger{new ThreadLogger{new StandardLogger{logger_decoration}},
                                                        new ThreadLogger{new StreamLogger{shared_ptr<std::ofstream>{new std::ofstream{"logs/test/bilog_demo.log", std::ios::out}}, logger_decoration}}}}};

    bi_logger->write(info, "All levels");
    bi_logger->write(level, "Level message");
    bi_logger->write(debug, "Debug message");
    bi_logger->write(info, "Info message");
    bi_logger->write(warning, "Warning message");
    bi_logger->write(error, "Error message");
    bi_logger->write(silence, "Silence message");

    bi_logger->write(info, "Min Warning level");
    bi_logger->set_min_level(warning);

    bi_logger->write(level, "Level message");
    bi_logger->write(debug, "Debug message");
    bi_logger->write(info, "Info message");
    bi_logger->write(warning, "Warning message");
    bi_logger->write(error, "Error message");
    bi_logger->write(silence, "Silence message");

    return 0;
}