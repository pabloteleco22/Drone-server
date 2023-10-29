#include "greeter.hpp"

using namespace simple_logger;

/** DefaultGreeter **/
string DefaultGreeter::greetings(const string &m) const {
    return "[Greetings] " + m;
}

/** ColorfulDefaultGreeter **/
string ColorfulDefaultGreeter::greetings(const string &m) const {
    return "\033[1;104m[Greetings]\033[0m " + m;
}

/** UserCustomGreeter **/
UserCustomGreeter::UserCustomGreeter(std::function<string(const string &)> custom_greetings) {
    this->custom_greetings = custom_greetings;
}

string UserCustomGreeter::greetings(const string &m) const {
    return custom_greetings(m);
}