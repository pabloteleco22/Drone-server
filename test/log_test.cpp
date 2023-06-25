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