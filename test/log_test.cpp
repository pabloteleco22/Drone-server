#include <gtest/gtest.h>
#include "../lib/log/log.hpp"

TEST(LogLevelTest, LevelHierarchy) {
    Level level;
    Debug debug;
    Info info;
    Warning warning;
    Error error;
    Silence silence;

    ASSERT_GT(silence, error);
    ASSERT_GT(error, warning);
    ASSERT_GT(warning, info);
    ASSERT_GT(info, debug);
    ASSERT_GT(debug, level);
}