#pragma once

#include <mavsdk/plugins/telemetry/telemetry.h>
#include <string>

using namespace mavsdk;

struct Flag {
    virtual ~Flag() {};
    virtual Telemetry::PositionNed get_flag_position() const = 0;
    virtual operator std::string() const = 0;
    virtual operator Telemetry::PositionNed() const = 0;
};

class RandomFlag : public Flag {
    private:
        std::shared_ptr<Telemetry::PositionNed> pos;

    public:
        static constexpr int default_north_m_max{10};
        static constexpr int default_north_m_min{-10};
        static constexpr int default_east_m_max{10};
        static constexpr int default_east_m_min{-10};
        static constexpr int default_down_m_max{10};
        static constexpr int default_down_m_min{-10};

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
        RandomFlag(MaxMin north_m={default_north_m_max, default_north_m_min},
                   MaxMin east_m={default_east_m_max, default_east_m_min},
                   MaxMin down_m={default_down_m_max, default_down_m_min});
        Telemetry::PositionNed get_flag_position() const override;
        operator std::string() const override;
        operator Telemetry::PositionNed() const override;
};

class FixedFlag : public Flag {
    private:
        static constexpr Telemetry::PositionNed default_pos{10.0f, 0.0f, -10.0f};
        std::shared_ptr<Telemetry::PositionNed> pos;

    public:
        FixedFlag();
        FixedFlag(Telemetry::PositionNed pos);

        Telemetry::PositionNed get_flag_position() const override;
        static Telemetry::PositionNed get_default_pos();

        operator std::string() const override;
        operator Telemetry::PositionNed() const override;
};