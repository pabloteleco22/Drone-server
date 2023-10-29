#include "levelfilter.hpp"

using namespace simple_logger;

bool DefaultFilter::filter(const Level &) const {
    return true;
}

/** UserCustomFilter **/
bool UserCustomFilter::filter(const Level &level) const {
    return custom_filter(level);
}