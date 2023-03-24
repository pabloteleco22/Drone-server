#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include "../flag/flag.hpp"
#include <typeinfo>

using testing::MatchesRegex;

const unsigned int MAX_REPETITIONS{100000};

/*** FixedFlag Tests ***/
TEST(FixedFlagTest, CheckDefaultPosition) {
    const Flag *fixed_flag{new FixedFlag};
    Telemetry::PositionNed expected_pos{FixedFlag::get_default_pos()};

    ASSERT_EQ(fixed_flag->get_flag_position(), expected_pos);

    delete fixed_flag;
}

TEST(FixedFlagTest, CheckCustomPosition) {
    const Telemetry::PositionNed expected_pos{9.0f, 1.0f, -20.0f};
    const Flag *fixed_flag{new FixedFlag{expected_pos}};

    ASSERT_EQ(fixed_flag->get_flag_position(), expected_pos);

    delete fixed_flag;
}

TEST(FixedFlagTest, CheckTelemetryPositionNedCasting) {
    const Telemetry::PositionNed expected_pos{10.0f, 0.0f, -10.0f};
    const Flag *fixed_flag{new FixedFlag{expected_pos}};

    ASSERT_EQ(static_cast<Telemetry::PositionNed>(*fixed_flag), expected_pos);

    delete fixed_flag;
}

TEST(FixedFlagTest, CheckStringCasting) {
    const Telemetry::PositionNed expected_pos{10.0f, 0.0f, -10.0f};
    const Flag *fixed_flag{new FixedFlag{expected_pos}};

    ASSERT_EQ(static_cast<std::string>(*fixed_flag),
            "North [m]: " + std::to_string(expected_pos.north_m) + "\n"
            + "East [m]: " + std::to_string(expected_pos.east_m) + "\n"
            + "Down [m]: " + std::to_string(expected_pos.down_m)
    );

    delete fixed_flag;
}

/*** RandomFlag Tests ***/
TEST(RandomFlagTest, CheckDefaultPosition) {
    const Flag *random_flag;

    for (unsigned int i = 0; i < MAX_REPETITIONS; ++i) {
        random_flag = new RandomFlag;
        const Telemetry::PositionNed position{random_flag->get_flag_position()};

        ASSERT_LE(position.north_m, RandomFlag::default_north_m_max);
        ASSERT_GE(position.north_m, RandomFlag::default_north_m_min);
        ASSERT_LE(position.east_m, RandomFlag::default_east_m_max);
        ASSERT_GE(position.east_m, RandomFlag::default_east_m_min);
        ASSERT_LE(position.down_m, RandomFlag::default_down_m_max);
        ASSERT_GE(position.down_m, RandomFlag::default_down_m_min);

        delete random_flag;
    }
}

TEST(RandomFlagTest, CheckCustomPosition) {
    const Flag *random_flag;
    RandomFlag::MaxMin north_m{7, 3};
    RandomFlag::MaxMin east_m{9, 9};
    RandomFlag::MaxMin down_m{4, 3};

    for (unsigned int i = 0; i < MAX_REPETITIONS; ++i) {
        random_flag = new RandomFlag{north_m, east_m, down_m};
        Telemetry::PositionNed position{random_flag->get_flag_position()};

        ASSERT_LE(position.north_m, north_m.get_max());
        ASSERT_GE(position.north_m, north_m.get_min());
        ASSERT_LE(position.east_m, east_m.get_max());
        ASSERT_GE(position.east_m, east_m.get_min());
        ASSERT_LE(position.down_m, down_m.get_max());
        ASSERT_GE(position.down_m, down_m.get_min());

        delete random_flag;
    }
}

TEST(RandomFlagTest, CheckTelemetryPositionNedCasting) {
    const Flag *random_flag{new RandomFlag};

    const std::type_info &instance_info{typeid(static_cast<Telemetry::PositionNed>(*random_flag))};
    const std::type_info &class_info{typeid(Telemetry::PositionNed)};

    ASSERT_EQ(instance_info.hash_code(), class_info.hash_code());

    delete random_flag;
}

TEST(RandomFlagTest, CheckStringCasting) {
    const Flag *random_flag{new RandomFlag};

    const std::string ret{static_cast<std::string>(*random_flag)};

    ASSERT_THAT(ret, MatchesRegex("^North \\[m\\]: -?[0-9]+\\.[0-9]+\nEast \\[m\\]: -?[0-9]+\\.[0-9]+\nDown \\[m\\]: -?[0-9]+\\.[0-9]+$"));

    delete random_flag;
}

/*** RandomFlag::MaxMin Tests ***/
TEST(RandomFlagMaxMinTest, CheckGetMaxGetMinCorrect1) {
    RandomFlag::MaxMin max_min{4, 2};

    ASSERT_GE(max_min.get_max(), max_min.get_min());
}

TEST(RandomFlagMaxMinTest, CheckGetMaxGetMinCorrect2) {
    RandomFlag::MaxMin max_min{4, 8};

    ASSERT_GE(max_min.get_max(), max_min.get_min());
}

TEST(RandomFlagMaxMinTest, CheckInterval) {
    const int max = 8;
    const int min = 4;
    RandomFlag::MaxMin max_min{max, min};

    ASSERT_EQ(max_min.get_interval(), max - min);
}