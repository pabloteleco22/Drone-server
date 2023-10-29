#pragma once

#include <functional>
#include <string>

using std::string;

namespace simple_logger {
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
};