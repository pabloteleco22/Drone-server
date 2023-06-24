/** Copyright (C) 2023  Pablo López Sedeño
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "../src/log/log.hpp"

#include <iostream>
#include <fstream>

using std::cout;
using std::endl;

using namespace simple_logger;

int main() {
    Debug debug;
    Info info;
    Warning warning;
    Error error;
    Critical critical;

    UserCustomFilter user_filter{
        [&warning](const Level &level) {
            return level == warning;
        }
    };

    cout << "Standard logger" << endl;
    StandardLoggerBuilder standard_logger_builder;

    Logger *standard_logger{standard_logger_builder.build()};
    cout << "All levels" << endl;
    standard_logger->write(debug, "Debug message");
    standard_logger->write(info, "Info message");
    standard_logger->write(warning, "Warning message");
    standard_logger->write(error, "Error message");
    standard_logger->write(critical, "Critical message");

    cout << endl << "Custom filter" << endl;
    standard_logger->set_level_filter(&user_filter);

    standard_logger->write(debug, "Debug message");
    standard_logger->write(info, "Info message");
    standard_logger->write(warning, "Warning message");
    standard_logger->write(error, "Error message");
    standard_logger->write(critical, "Critical message");

    delete standard_logger;

    cout << endl << "Standard stream logger" << endl;
    StreamLoggerBuilder stream_logger_builder(&cout);

    Logger *standard_stream_logger{stream_logger_builder.build()};
    cout << "All levels" << endl;
    standard_stream_logger->write(debug, "Debug message");
    standard_stream_logger->write(info, "Info message");
    standard_stream_logger->write(warning, "Warning message");
    standard_stream_logger->write(error, "Error message");
    standard_stream_logger->write(critical, "Critical message");

    cout << endl << "Custom filter" << endl;
    standard_stream_logger->set_level_filter(&user_filter);

    standard_stream_logger->write(debug, "Debug message");
    standard_stream_logger->write(info, "Info message");
    standard_stream_logger->write(warning, "Warning message");
    standard_stream_logger->write(error, "Error message");
    standard_stream_logger->write(critical, "Critical message");

    delete standard_stream_logger;

    std::ofstream log_demo_stream{"logs/test/log_demo.log", std::ios::out};
    stream_logger_builder.reset_config()
                         .set_stream(&log_demo_stream);

    Logger *stream_logger{stream_logger_builder.build()};
    stream_logger->write(info, "All levels");
    stream_logger->write(debug, "Debug message");
    stream_logger->write(info, "Info message");
    stream_logger->write(warning, "Warning message");
    stream_logger->write(error, "Error message");
    stream_logger->write(critical, "Critical message");

    stream_logger->write(info, "Custom filter");
    stream_logger->set_level_filter(&user_filter);

    stream_logger->write(debug, "Debug message");
    stream_logger->write(info, "Info message");
    stream_logger->write(warning, "Warning message");
    stream_logger->write(error, "Error message");
    stream_logger->write(critical, "Critical message");

    delete stream_logger;

    cout << endl << "Thread standard logger" << endl;
    TimedLoggerDecoration timed_logger_decoration;
    standard_logger_builder.reset_config()
                           .set_decoration(&timed_logger_decoration);
    Logger *thread_standard_logger{standard_logger_builder.build()};
    ThreadLogger thread_logger{thread_standard_logger};

    cout << "All levels" << endl;
    thread_logger.write(debug, "Debug message");
    thread_logger.write(info, "Info message");
    thread_logger.write(warning, "Warning message");
    thread_logger.write(error, "Error message");
    thread_logger.write(critical, "Critical message");

    cout << endl << "Custom filter" << endl;
    thread_logger.set_level_filter(&user_filter);

    thread_logger.write(debug, "Debug message");
    thread_logger.write(info, "Info message");
    thread_logger.write(warning, "Warning message");
    thread_logger.write(error, "Error message");
    thread_logger.write(critical, "Critical message");

    delete thread_standard_logger;


    std::ofstream thread_log_demo_stream{"logs/test/thread_log_demo.log", std::ios::out};
    stream_logger_builder.reset_config()
                         .set_decoration(&timed_logger_decoration)
                         .set_stream(&thread_log_demo_stream);

    Logger *thread_stream_logger{stream_logger_builder.build()};
    thread_logger.set_logger(thread_stream_logger);

    thread_logger.write(info, "All levels");
    thread_logger.write(debug, "Debug message");
    thread_logger.write(info, "Info message");
    thread_logger.write(warning, "Warning message");
    thread_logger.write(error, "Error message");
    thread_logger.write(critical, "Critical message");

    thread_logger.write(info, "Custom filter");
    thread_logger.set_level_filter(&user_filter);

    thread_logger.write(debug, "Debug message");
    thread_logger.write(info, "Info message");
    thread_logger.write(warning, "Warning message");
    thread_logger.write(error, "Error message");
    thread_logger.write(critical, "Critical message");

    delete thread_stream_logger;


    cout << endl << "Bi logger" << endl;

    UserCustomGreeter custom_greeter{[](const string &m) {
        HourLoggerDecoration decoration;

        return "\033[1;104m[" + decoration.get_decoration() + "Greetings]\033[0m " + m;
    }};

    HourLoggerDecoration logger_decoration;

    standard_logger_builder.reset_config()
                           .set_decoration(&logger_decoration)
                           .set_greeter(&custom_greeter)
                           .set_greeting_string("Starting thread standard logger");

    std::ofstream bilog_demo_stream{"logs/test/bilog_demo.log", std::ios::out};
    stream_logger_builder.reset_config()
                         .set_decoration(&logger_decoration)
                         .set_stream(&bilog_demo_stream);

    Logger *std_logger{standard_logger_builder.build()};
    Logger *str_logger{stream_logger_builder.build()};

    ThreadLogger bilog_thread_std_logger{std_logger};
    ThreadLogger bilog_thread_str_logger{str_logger};

    BiLogger bi_logger{&bilog_thread_std_logger, &bilog_thread_str_logger};

    bi_logger.write(info, "All levels");
    bi_logger.write(debug, "Debug message");
    bi_logger.write(info, "Info message");
    bi_logger.write(warning, "Warning message");
    bi_logger.write(error, "Error message");
    bi_logger.write(critical, "Critical message");

    bi_logger.write(info, "Custom filter");
    bi_logger.set_level_filter(&user_filter);

    bi_logger.write(debug, "Debug message");
    bi_logger.write(info, "Info message");
    bi_logger.write(warning, "Warning message");
    bi_logger.write(error, "Error message");
    bi_logger.write(critical, "Critical message");

    delete std_logger;
    delete str_logger;

    return 0;
}