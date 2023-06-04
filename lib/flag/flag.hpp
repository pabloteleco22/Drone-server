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
    public:
        class MaxMin {
            private:
                double max;
                double min;
            
            public:
                MaxMin(double n1, double n2);
                MaxMin(const MaxMin &other);
                MaxMin &operator=(const MaxMin &other);
                double get_max() const;
                double get_min() const;
                double get_interval() const;
        };

        inline static const MaxMin default_latitude_deg_interval{10, -10};
        inline static const MaxMin default_longitude_deg_interval{10, -10};

        RandomFlag(const MaxMin &latitude_deg_interval=default_latitude_deg_interval,
                   const MaxMin &longitude_deg_interval=default_longitude_deg_interval);
        Position get_flag_position() const override;
        operator std::string() const override;

    private:
        Position pos;
        MaxMin latitude_deg_interval{default_latitude_deg_interval};
        MaxMin longitude_deg_interval{default_longitude_deg_interval};
};

class FixedFlag : public Flag {
    private:
        Position pos;

    public:
        FixedFlag();
        FixedFlag(Position pos);
        FixedFlag(const double latitude_deg, const double longitude_deg);

        Position get_flag_position() const override;

        operator std::string() const override;

        static constexpr Position default_pos{10.0, 0.0};
};