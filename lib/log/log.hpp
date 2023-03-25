#pragma once

#include <string>
#include <memory>
#include <chrono>

using std::string;
using std::shared_ptr;

struct Level {
    Level();
    Level(Level *other);
    Level(shared_ptr<Level> other);
    bool operator>=(const Level &other) const;
    string get_color() const;
    string get_level_name() const;
    bool is_printable() const;

    protected:
        unsigned short level_number{0};
        string color{"\033[0m"};
        string level_name{"Level"};
        bool printable{false};
};

struct Debug : public Level {
    using Level::Level;
    Debug() {
        level_number = 51;
        color = "\033[34m";
        level_name = "Debug";
        printable = true;
    }
};

struct Info : public Level {
    using Level::Level;
    Info() {
        level_number = 102;
        color = "\033[32m";
        level_name = "Info";
        printable = true;
    }
};

struct Warning : public Level {
    using Level::Level;
    Warning() {
        level_number = 153;
        color = "\033[33m";
        level_name = "Warning";
        printable = true;
    }
};

struct Error : public Level {
    using Level::Level;
    Error() {
        level_number = 204;
        color = "\033[31m";
        level_name = "Error";
        printable = true;
    }
};

struct Silence : public Level {
    using Level::Level;
    Silence() {
        level_number = 255;
        level_name = "Silence";
    }
};

struct Logger {
    Logger();
    Logger(Logger *other);
    Logger(shared_ptr<Logger> other);
    virtual void write(shared_ptr<Level> level, const string &message) = 0;
    void set_min_level(shared_ptr<Level> level);

    protected:
        shared_ptr<Level> min_level;
        double get_timestamp();

    private:
        std::chrono::time_point<std::chrono::steady_clock> start_time;
};

struct StandardLogger : public Logger {
    using Logger::Logger;
    void write(shared_ptr<Level> level, const string &message) override;
};
