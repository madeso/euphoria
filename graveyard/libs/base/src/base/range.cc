#include "base/range.h"

namespace eu
{
    float
    from_01f(float lower_bound, float upper_bound, float value)
    {
        return value * (upper_bound - lower_bound) + lower_bound;
    }

    template <>
    float
    from_01(const Range<float>& range, float value)
    {
        return from_01f
        (
            range.lower_bound,
            range.upper_bound,
            value
        );
    }
}

