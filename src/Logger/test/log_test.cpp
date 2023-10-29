#include <gtest/gtest.h>
#include "level.hpp"

using namespace simple_logger;

TEST(LogLevelTest, LevelHierarchy) {
    Debug debug;
    Info info;
    Warning warning;
    Error error;

    ASSERT_GT(error, warning);
    ASSERT_GT(warning, info);
    ASSERT_GT(info, debug);
}