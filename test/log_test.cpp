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