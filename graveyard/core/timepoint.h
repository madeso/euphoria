#pragma once


#include <chrono>

namespace eu::core
{
    using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

    TimePoint
    get_current_timepoint();

    float
    get_seconds_between(const TimePoint& start, const TimePoint& end);
}
