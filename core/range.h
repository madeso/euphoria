#ifndef EUPHORIA_RANGE_H
#define EUPHORIA_RANGE_H

#include <vector>

#include "core/angle.h"
#include "core/assert.h"

namespace euphoria::core
{
    template <typename T>
    struct Range
    {
        Range(T min, T max) : lower_bound(min), upper_bound(max)
        {
            ASSERTX(lower_bound <= upper_bound, upper_bound, lower_bound);
        }

        explicit Range(T max) : lower_bound(0), upper_bound(max)
        {
            ASSERTX(lower_bound <= upper_bound, upper_bound, lower_bound);
        }

        T lower_bound;
        T upper_bound;
    };

    template <typename T>
    Range<T>
    MakeRange(T min, T max)
    {
        return {min, max};
    }

    template <typename T>
    Range<T>
    MakeRange(T max)
    {
        return Range<T>(max);
    }

    template <typename T>
    Range<int>
    MakeRange(const std::vector<T>& v)
    {
        ASSERT(!v.empty());
        return MakeRange<int>(v.size() - 1);
    }

    const Range<float>&
    R01();
    const Range<float>&
    R11();

    template <typename T>
    T
    From01(const Range<T>& range, float value)
    {
        return value * (range.upper_bound - range.lower_bound)
               + range.lower_bound;
    }

    template <typename T>
    float
    To01(const Range<T>& range, T value)
    {
        return (value - range.lower_bound)
               / (range.upper_bound - range.lower_bound);
    }

    // inclusive
    template <typename T>
    T
    Get360Angular(const Range<T>& range, float value)
    {
        const float half_difference
                = (range.upper_bound - range.lower_bound) / 2.0f;
        return range.lower_bound + half_difference
               - half_difference * Cos(Angle::FromPercentOf360(value));
    }

    template <typename T, typename F>
    T
    RemapTo(const Range<F>& from, const Range<T>& to, F value)
    {
        return From01(to, To01(from, value));
    }

    // includsive, both min and max are included
    template <typename T>
    bool
    IsWithin(const Range<T>& range, T value)
    {
        return value >= range.lower_bound && value <= range.upper_bound;
    }

    // inclusive, both min and max are included
    template <typename T>
    T
    KeepWithin(const Range<T>& range, T value)
    {
        if (value > range.upper_bound)
        {
            return range.upper_bound;
        }
        if (value < range.lower_bound)
        {
            return range.lower_bound;
        }

        return value;
    }

    template <typename T>
    T
    Wrap(const Range<T>& range, T value)
    {
        const T diff = range.upper_bound - range.lower_bound;
        ASSERT(diff > 0);
        T wrapped = value - range.lower_bound;
        while (wrapped < 0)
        {
            wrapped += diff;
        }
        while (wrapped > diff)
        {
            wrapped -= diff;
        }
        return range.lower_bound + wrapped;
    }

}  // namespace euphoria::core

#endif  // EUPHORIA_RANGE_H
