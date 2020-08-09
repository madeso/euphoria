#ifndef EUPHORIA_CORE_TIMEPOINT_H
#define EUPHORIA_CORE_TIMEPOINT_H

#include <chrono>

namespace euphoria::core
{
    using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

    TimePoint
    Now();

    double
    SecondsBetween(const TimePoint& start, const TimePoint& end);
}

#endif  // EUPHORIA_CORE_TIMEPOINT_H

