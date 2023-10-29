#include "decoration.hpp"

#include <iomanip>

using namespace simple_logger;

/** VoidLoggerDecoration **/
string VoidLoggerDecoration::get_decoration() const {
    return "";
}

DecorationBundler::DecorationBundler(std::vector<const LoggerDecoration*> &decoration_list, const string separator) : separator{separator} {
    this->decoration_list = &decoration_list;
}

string DecorationBundler::get_decoration() const {
    string decoration{""};
    
    auto it{decoration_list->begin()};
    
    if (it != decoration_list->end()) {
        decoration += (*it)->get_decoration();

        while (it + 1 != decoration_list->end()) {
            ++it;
            decoration += separator + (*it)->get_decoration();
        }
    }
    
    return decoration;
}

PackDecoration::PackDecoration(const LoggerDecoration &logger_decoration, const string begin, const string end) :
    logger_decoration{&logger_decoration}, begin{begin}, end{end} {}

string PackDecoration::get_decoration() const {
    return begin + logger_decoration->get_decoration() + end;
}

/** TimedLoggerDecoration **/
TimedLoggerDecoration::TimedLoggerDecoration() : LoggerDecoration() {
    start_time = std::chrono::steady_clock::now();
}

TimedLoggerDecoration::TimedLoggerDecoration(TimedLoggerDecoration &other) : LoggerDecoration() {
    start_time = other.start_time;
}

string TimedLoggerDecoration::get_decoration() const {
    std::ostringstream os;
    os << std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start_time).count();
    
    return os.str();
}

/** HourLoggerDecoration **/
string HourLoggerDecoration::get_decoration() const {
    std::ostringstream os;

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::time_t t_now = std::chrono::system_clock::to_time_t(now);
    os << std::put_time(std::localtime(&t_now), "%T");

    return os.str();
}
