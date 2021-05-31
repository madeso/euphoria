#include "core/timepoint.h"

namespace euphoria::core
{
    time_point
    time_point_now()
    {
        return std::chrono::steady_clock::now();
    }

    double
    get_seconds_between(const time_point& start, const time_point& end)
    {
        const std::chrono::duration<double> diff = end-start;
        return diff.count();
    }
}

