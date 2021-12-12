#pragma once


#include <chrono>

namespace euphoria::core
{
    using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

    TimePoint
    time_point_now();

    float
    get_seconds_between(const TimePoint& start, const TimePoint& end);
}
