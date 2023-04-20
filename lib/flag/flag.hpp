#pragma once

#include <mavsdk/plugins/telemetry/telemetry.h>
#include <string>

using namespace mavsdk;

struct Flag {
    struct Position {
        double latitude_deg;
        double longitude_deg;

        bool operator==(const Position &other) const {
            return latitude_deg == other.latitude_deg and longitude_deg == other.longitude_deg;
        }
    };

    virtual ~Flag() {};
    virtual Position get_flag_position() const = 0;
    virtual operator std::string() const = 0;
};

class RandomFlag : public Flag {
    private:
        Position pos;

    public:
        class MaxMin {
            private:
                int max;
                int min;
            
            public:
                MaxMin(int n1, int n2);
                int get_max() const;
                int get_min() const;
                int get_interval() const;
        };

        inline static const MaxMin default_latitude_deg{10, -10};
        inline static const MaxMin default_longitude_deg{10, -10};

        RandomFlag(MaxMin latitude_deg=default_latitude_deg, MaxMin longitude_deg=default_longitude_deg);
        Position get_flag_position() const override;
        operator std::string() const override;
};

class FixedFlag : public Flag {
    private:
        static constexpr Position default_pos{10.0, 0.0};
        Position pos;

    public:
        FixedFlag();
        FixedFlag(Position pos);
        FixedFlag(const double latitude_deg, const double longitude_deg);

        Position get_flag_position() const override;
        static Position get_default_pos();

        operator std::string() const override;
};