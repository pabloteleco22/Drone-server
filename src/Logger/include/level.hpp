#pragma once

#include <string>

using std::string;

namespace simple_logger {
    struct Level {
        Level() {};
        Level(const Level *other);
        virtual ~Level() = 0;
        bool operator>=(const Level &other) const;
        bool operator>(const Level &other) const;
        bool operator<=(const Level &other) const;
        bool operator<(const Level &other) const;
        bool operator==(const Level &other) const;
        bool operator!=(const Level &other) const;
        string get_color() const;
        string get_level_name() const;

    protected:
        unsigned short level_number{ 0 };
        string color{ "\033[0m" };
        string level_name{ "Level" };
    };

    struct Debug : public Level {
        using Level::Level;
        using Level::operator>=;
        using Level::operator>;
        using Level::operator<=;
        using Level::operator<;
        using Level::operator==;
        using Level::operator!=;
        Debug() {
            level_number = 51;
            color = "\033[1;32m";
            level_name = "Debug";
        }
    };

    struct Info : public Level {
        using Level::Level;
        using Level::operator>=;
        using Level::operator>;
        using Level::operator<=;
        using Level::operator<;
        using Level::operator==;
        using Level::operator!=;
        Info() {
            level_number = 102;
            color = "\033[1;34m";
            level_name = "Info";
        }
    };

    struct Warning : public Level {
        using Level::Level;
        using Level::operator>=;
        using Level::operator>;
        using Level::operator<=;
        using Level::operator<;
        using Level::operator==;
        using Level::operator!=;
        Warning() {
            level_number = 153;
            color = "\033[1;33m";
            level_name = "Warn";
        }
    };

    struct Error : public Level {
        using Level::Level;
        using Level::operator>=;
        using Level::operator>;
        using Level::operator<=;
        using Level::operator<;
        using Level::operator==;
        using Level::operator!=;
        Error() {
            level_number = 204;
            color = "\033[1;31m";
            level_name = "Error";
        }
    };

    struct Critical : public Level {
        using Level::Level;
        using Level::operator>=;
        using Level::operator>;
        using Level::operator<=;
        using Level::operator<;
        using Level::operator==;
        using Level::operator!=;
        Critical() {
            level_number = 229;
            color = "\033[1;95m";
            level_name = "Critical";
        }
    };
};