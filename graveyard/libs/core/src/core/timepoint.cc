#include "core/timepoint.h"

namespace eu::core
{
    TimePoint
    get_current_timepoint()
    {
        return std::chrono::steady_clock::now();
    }

    float
    get_seconds_between(const TimePoint& start, const TimePoint& end)
    {
        const std::chrono::duration<float> diff = end-start;
        return diff.count();
    }
}

