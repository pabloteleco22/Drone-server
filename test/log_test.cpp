/** Copyright (C) 2023  Pablo López Sedeño
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>
#include "../src/log/log.hpp"

using namespace simple_logger;

TEST(LogLevelTest, LevelHierarchyGreaterThan) {
    Debug debug;
    Info info;
    Warning warning;
    Error error;
    Critical critical;

    ASSERT_GT(critical, error);
    ASSERT_GT(error, warning);
    ASSERT_GT(warning, info);
    ASSERT_GT(info, debug);
}

TEST(LogLevelTest, LevelHierarchyLessThan) {
    Debug debug;
    Info info;
    Warning warning;
    Error error;
    Critical critical;

    ASSERT_LT(debug, info);
    ASSERT_LT(info, warning);
    ASSERT_LT(warning, error);
    ASSERT_LT(error, critical);
}

TEST(LogLevelTest, LevelHierarchyGreaterThanOrEqualTo) {
    Debug debug;
    Info info;
    Warning warning;
    Error error;
    Critical critical;

    ASSERT_GE(critical, error);
    ASSERT_GE(error, warning);
    ASSERT_GE(warning, info);
    ASSERT_GE(info, debug);
    ASSERT_GE(critical, critical);
    ASSERT_GE(error, error);
    ASSERT_GE(warning, warning);
    ASSERT_GE(info, info);
    ASSERT_GE(debug, debug);
}

TEST(LogLevelTest, LevelHierarchyLessThanOrEqualTo) {
    Debug debug;
    Info info;
    Warning warning;
    Error error;
    Critical critical;

    ASSERT_LE(debug, info);
    ASSERT_LE(info, warning);
    ASSERT_LE(warning, error);
    ASSERT_LE(error, critical);
    ASSERT_LE(critical, critical);
    ASSERT_LE(error, error);
    ASSERT_LE(warning, warning);
    ASSERT_LE(info, info);
    ASSERT_LE(debug, debug);
}

TEST(LogLevelTest, LevelEquality) {
    Debug debug1;
    Debug debug2;

    ASSERT_EQ(debug1, debug2);
}

TEST(LogLevelTest, LevelInequality) {
    Debug debug;
    Info info;

    ASSERT_NE(debug, info);
}