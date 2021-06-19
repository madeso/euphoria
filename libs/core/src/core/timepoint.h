#pragma once


#include <chrono>

namespace euphoria::core
{
    using time_point = std::chrono::time_point<std::chrono::steady_clock>;

    time_point
    time_point_now();

    float
    get_seconds_between(const time_point& start, const time_point& end);
}
