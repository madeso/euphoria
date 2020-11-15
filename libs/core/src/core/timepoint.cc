#include "core/timepoint.h"

namespace euphoria::core
{
    TimePoint
    Now()
    {
        return std::chrono::steady_clock::now();
    }

    double
    SecondsBetween(const TimePoint& start, const TimePoint& end)
    {
        const std::chrono::duration<double> diff = end-start;
        return diff.count();
    }
}

