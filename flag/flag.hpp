#pragma once

#include <mavsdk/plugins/telemetry/telemetry.h>
#include <string>

using namespace mavsdk;

struct Flag {
    virtual Telemetry::PositionNed get_flag_position() const = 0;
    virtual operator std::string() const = 0;
    virtual operator Telemetry::PositionNed() const = 0;
};

class RandomFlag : public Flag {
    private:
        Telemetry::PositionNed *pos;

    public:
        class MaxMin {
            private:
                int max;
                int min;
            
            public:
                MaxMin(int n1, int n2);
                int get_max();
                int get_min();
                int get_interval();
        };
        RandomFlag(MaxMin north_m={10, -10}, MaxMin east_m={10, -10}, MaxMin down_m={10, -10});
        ~RandomFlag();
        Telemetry::PositionNed get_flag_position() const;
        operator std::string() const;
        operator Telemetry::PositionNed() const;
};

class FixedFlag : public Flag {
    private:
        Telemetry::PositionNed pos{10.0f, 0.0f, -10.0f};

    public:
        Telemetry::PositionNed get_flag_position() const;
        operator std::string() const;
        operator Telemetry::PositionNed() const;
};