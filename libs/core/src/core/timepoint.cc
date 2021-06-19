#include "core/timepoint.h"

namespace euphoria::core
{
    time_point
    time_point_now()
    {
        return std::chrono::steady_clock::now();
    }

    float
    get_seconds_between(const time_point& start, const time_point& end)
    {
        const std::chrono::duration<float> diff = end-start;
        return diff.count();
    }
}

