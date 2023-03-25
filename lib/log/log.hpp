#pragma once

#include <string>
#include <array>
#include <memory>

using std::string;

struct Logger {
    struct Level {
        bool operator>=(const Level &other) const;
        string get_color();

        protected:
            unsigned short level_number{0};
            string color{"\033[34m"};
    };

    Logger();
    virtual void write(Level *level, const string &message) = 0;
    void set_min_level(Level *level);

    protected:
        Level *min_level;
};

struct StandardLogger : public Logger {
    void write(Level *level, const string &message) override;
};

struct Debug : public Logger::Level {
};

struct Info : public Logger::Level {
    unsigned short level_number{1};
    Info() { color = "\033[32m"; }
};

struct Warning : public Logger::Level {
    unsigned short level_number{2};
    Warning() { color = "\033[33m"; }
};

struct Error : public Logger::Level {
    unsigned short level_number{3};
    Error() { color = "\033[31m"; }
};

struct Silence : public Logger::Level {
    unsigned short level_number{4};
    Silence() { color = "\033[8m"; }
};