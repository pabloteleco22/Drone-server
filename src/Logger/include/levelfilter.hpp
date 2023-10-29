#pragma once

#include "level.hpp"

#include <functional>

namespace simple_logger {
struct LevelFilter {
    virtual bool filter(const Level &level) const = 0;
};

struct DefaultFilter : public LevelFilter {
    bool filter(const Level &level) const override;
};

struct UserCustomFilter : public LevelFilter {
    UserCustomFilter(std::function<bool(const Level &level)> custom_filter) :
                                                            custom_filter{custom_filter} {}

    bool filter(const Level &level) const override;

    private:
        std::function<bool(const Level &level)> custom_filter;
};
};