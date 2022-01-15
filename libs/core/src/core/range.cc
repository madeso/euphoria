#include "core/range.h"

namespace euphoria::core
{
    float
    from01f(float lower_bound, float upper_bound, float value)
    {
        return value * (upper_bound - lower_bound) + lower_bound;
    }

    template <>
    float
    from01(const Range<float>& range, float value)
    {
        return from01f
        (
            range.lower_bound,
            range.upper_bound,
            value
        );
    }
}

