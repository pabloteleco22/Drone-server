/**
 * The MIT License (MIT)
 * Copyright (c) 2023 Pablo López Sedeño
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include "../src/flag/flag.hpp"
#include <typeinfo>

using testing::MatchesRegex;

const unsigned int MAX_REPETITIONS{ 100000 };

/*** FixedFlag Tests ***/
TEST(FixedFlagTest, CheckDefaultPosition) {
    const Flag *fixed_flag{ new FixedFlag };
    Flag::Position expected_pos{ FixedFlag::default_pos };

    ASSERT_EQ(fixed_flag->get_flag_position(), expected_pos);

    delete fixed_flag;
}

TEST(FixedFlagTest, CheckCustomPosition) {
    const Flag::Position expected_pos{ 9.0, 1.0 };
    const Flag *fixed_flag{ new FixedFlag{expected_pos} };

    ASSERT_EQ(fixed_flag->get_flag_position(), expected_pos);

    delete fixed_flag;
}

TEST(FixedFlagTest, CheckStringCasting) {
    const Flag::Position expected_pos{ 10.0, 0.0 };
    const Flag *fixed_flag{ new FixedFlag{expected_pos} };

    ASSERT_EQ(static_cast<std::string>(*fixed_flag),
        "Latitude [deg]: " + std::to_string(expected_pos.latitude_deg) + "\n"
        + "Longitude [deg]: " + std::to_string(expected_pos.longitude_deg)
    );

    delete fixed_flag;
}

/*** RandomFlag Tests ***/
TEST(RandomFlagTest, CheckDefaultPosition) {
    const Flag *random_flag;

    for (unsigned int i = 0; i < MAX_REPETITIONS; ++i) {
        random_flag = new RandomFlag{ false };
        const Flag::Position position{ random_flag->get_flag_position() };

        ASSERT_LE(position.latitude_deg, RandomFlag::default_latitude_deg_interval.get_max());
        ASSERT_GE(position.latitude_deg, RandomFlag::default_latitude_deg_interval.get_min());
        ASSERT_LE(position.longitude_deg, RandomFlag::default_longitude_deg_interval.get_max());
        ASSERT_GE(position.longitude_deg, RandomFlag::default_longitude_deg_interval.get_min());

        delete random_flag;
    }
}

TEST(RandomFlagTest, CheckCustomPosition) {
    const Flag *random_flag;
    RandomFlag::MaxMin latitude_deg{ 7, 3 };
    RandomFlag::MaxMin longitude_deg{ 9, 9 };

    for (unsigned int i = 0; i < MAX_REPETITIONS; ++i) {
        random_flag = new RandomFlag{ latitude_deg, longitude_deg, false };
        Flag::Position position{ random_flag->get_flag_position() };

        ASSERT_LE(position.latitude_deg, latitude_deg.get_max());
        ASSERT_GE(position.latitude_deg, latitude_deg.get_min());
        ASSERT_LE(position.longitude_deg, longitude_deg.get_max());
        ASSERT_GE(position.longitude_deg, longitude_deg.get_min());

        delete random_flag;
    }
}

TEST(RandomFlagTest, CheckStringCasting) {
    const Flag *random_flag;

    for (unsigned int i = 0; i < MAX_REPETITIONS; ++i) {
        random_flag = new RandomFlag{ false };

        const std::string ret{ static_cast<std::string>(*random_flag) };

        ASSERT_THAT(ret, MatchesRegex("^Latitude \\[deg\\]: -?[0-9]+(\\.[0-9]+)?\nLongitude \\[deg\\]: -?[0-9]+(\\.[0-9]+)?$"));

        delete random_flag;
    }
}

/*** RandomFlag::MaxMin Tests ***/
TEST(RandomFlagMaxMinTest, CheckGetMaxGetMinCorrect1) {
    RandomFlag::MaxMin max_min{ 4, 2 };

    ASSERT_GE(max_min.get_max(), max_min.get_min());
}

TEST(RandomFlagMaxMinTest, CheckGetMaxGetMinCorrect2) {
    RandomFlag::MaxMin max_min{ 4, 8 };

    ASSERT_GE(max_min.get_max(), max_min.get_min());
}

TEST(RandomFlagMaxMinTest, CheckInterval) {
    const int max = 8;
    const int min = 4;
    RandomFlag::MaxMin max_min{ max, min };

    ASSERT_EQ(max_min.get_interval(), max - min);
}

/*** RandomFlagPoly Tests ***/
TEST(RandomFlagPolyTest, CheckDefaultPosition) {
    Polygon poly;

    for (Point vertex : RandomFlagPoly::default_polygon_vertices) {
        poly.push_back(vertex);
    }

    for (unsigned int i = 0; i < MAX_REPETITIONS; ++i) {
        Flag::Position pos{ RandomFlagPoly{false}.get_flag_position() };
        Point point{ pos.latitude_deg, pos.longitude_deg };
        ASSERT_TRUE(poly.is_point_inside(point));
    }
}

TEST(RandomFlagPolyTest, CheckCustomPosition) {
    Polygon poly;
    poly.push_back({ 0, 1 });
    poly.push_back({ 1, 0 });
    poly.push_back({ 0, -1 });
    poly.push_back({ -1, 0 });

    for (unsigned int i = 0; i < MAX_REPETITIONS; ++i) {
        Flag::Position pos{ RandomFlagPoly{poly, false}.get_flag_position() };
        Point point{ pos.latitude_deg, pos.longitude_deg };
        ASSERT_TRUE(poly.is_point_inside(point));
    }
}

TEST(RandomFlagPolyTest, CheckCustomPosition2) {
    Polygon poly;
    poly.push_back({ -20, 10 });
    poly.push_back({ -10, 30 });
    poly.push_back({ 7.5, 30 });
    poly.push_back({ 15, 10 });
    poly.push_back({ 22.5, 20 });
    poly.push_back({ 30, -10 });
    poly.push_back({ -25, -10 });
    poly.push_back({ -40, -5 });
    poly.push_back({ -30, 20 });

    for (unsigned int i = 0; i < MAX_REPETITIONS; ++i) {
        Flag::Position pos{ RandomFlagPoly{poly, false}.get_flag_position() };
        Point point{ pos.latitude_deg, pos.longitude_deg };
        ASSERT_TRUE(poly.is_point_inside(point));
    }
}