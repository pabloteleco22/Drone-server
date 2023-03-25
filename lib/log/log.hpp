#pragma once

#include <string>
#include <array>
#include <memory>

using std::string;
using std::shared_ptr;

struct Level {
    Level();
    Level(Level *other);
    Level(shared_ptr<Level> other);
    bool operator>=(const Level &other) const;
    string get_color();

    protected:
        unsigned short level_number{0};
        string color{"\033[0m"};
};

struct Debug : public Level {
    using Level::Level;
    Debug() { color = "\033[34m"; level_number = 51; }
};

struct Info : public Level {
    using Level::Level;
    Info() { color = "\033[32m"; level_number = 102; }
};

struct Warning : public Level {
    using Level::Level;
    Warning() { color = "\033[33m"; level_number = 153; }
};

struct Error : public Level {
    using Level::Level;
    Error() { color = "\033[31m"; level_number = 204; }
};

struct Silence : public Level {
    using Level::Level;
    Silence() { color = "\033[8m"; level_number = 255; }
};

struct Logger {
    Logger();
    Logger(Logger *other);
    Logger(shared_ptr<Logger> other);
    virtual void write(shared_ptr<Level> level, const string &message) = 0;
    void set_min_level(shared_ptr<Level> level);

    protected:
        shared_ptr<Level> min_level;
};

struct StandardLogger : public Logger {
    using Logger::Logger;
    void write(shared_ptr<Level> level, const string &message) override;
};
