#include "../lib/log/log.hpp"

#include <iostream>
#include <fstream>

using std::cout;
using std::endl;
using std::shared_ptr;
using std::make_shared;

int main() {
    shared_ptr<Logger> standard_logger{make_shared<StandardLogger>(new StandardLogger)};

    shared_ptr<Level> level{make_shared<Level>(new Level)};
    shared_ptr<Level> debug{make_shared<Debug>(new Debug)};
    shared_ptr<Level> info{make_shared<Info>(new Info)};
    shared_ptr<Level> warning{make_shared<Warning>(new Warning)};
    shared_ptr<Level> error{make_shared<Error>(new Error)};
    shared_ptr<Level> silence{make_shared<Silence>(new Silence)};

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

    shared_ptr<Logger> standard_stream_logger{make_shared<StreamLogger>(new StreamLogger{shared_ptr<std::ostream>(&cout, [](void *) {})})};

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

    shared_ptr<Logger> stream_logger{make_shared<StreamLogger>(new StreamLogger{shared_ptr<std::ofstream>(new std::ofstream{"build/log_demo.log", std::ios::out})})};

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

    cout << "Bi logger" << endl;
    shared_ptr<Logger> bi_logger{make_shared<BiLogger>(new BiLogger{shared_ptr<std::ofstream>(new std::ofstream{"build/bilog_demo.log", std::ios::out})})};

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

    cout << "Thread standard logger" << endl;
    shared_ptr<Logger> thread_standard_logger{make_shared<ThreadStandardLogger>(new ThreadStandardLogger)};

    thread_standard_logger->write(info, "All levels");
    thread_standard_logger->write(level, "Level message");
    thread_standard_logger->write(debug, "Debug message");
    thread_standard_logger->write(info, "Info message");
    thread_standard_logger->write(warning, "Warning message");
    thread_standard_logger->write(error, "Error message");
    thread_standard_logger->write(silence, "Silence message");

    cout << "Min Warning level" << endl;
    thread_standard_logger->set_min_level(warning);

    thread_standard_logger->write(level, "Level message");
    thread_standard_logger->write(debug, "Debug message");
    thread_standard_logger->write(info, "Info message");
    thread_standard_logger->write(warning, "Warning message");
    thread_standard_logger->write(error, "Error message");
    thread_standard_logger->write(silence, "Silence message");

    return 0;
}