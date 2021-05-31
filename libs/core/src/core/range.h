#pragma once


#include <vector>

#include "core/angle.h"
#include "core/assert.h"

namespace euphoria::core
{
    template <typename T>
    struct range
    {
        range(T min, T max) : lower_bound(min), upper_bound(max)
        {
            ASSERTX(lower_bound <= upper_bound, upper_bound, lower_bound);
        }

        explicit range(T max) : lower_bound(0), upper_bound(max)
        {
            ASSERTX(lower_bound <= upper_bound, upper_bound, lower_bound);
        }

        [[nodiscard]] T
        get_distance() const
        {
            return upper_bound - lower_bound;
        }

        // the yes, Im sure variant
        constexpr range(T min, T max, int*) : lower_bound(min), upper_bound(max)
        {
        }

        T lower_bound;
        T upper_bound;
    };

    template <typename T>
    range<T>
    make_range(T min, T max)
    {
        return {min, max};
    }

    template <typename T>
    range<T>
    make_range(T max)
    {
        return range<T>(max);
    }

    template <typename T>
    range<int>
    make_range(const std::vector<T>& v)
    {
        ASSERT(!v.empty());
        return make_range<int>(v.size() - 1);
    }

    constexpr range<float> r01 = { 0.0f, 1.0f, nullptr};
    constexpr range<float> r11 = { -1.0f, 1.0f, nullptr};

    template <typename T>
    T
    from01(const range<T>& range, float value)
    {
        return value * (range.upper_bound - range.lower_bound)
               + range.lower_bound;
    }

    template <typename T>
    float
    to01(const range<T>& range, T value)
    {
        return (value - range.lower_bound)
               / (range.upper_bound - range.lower_bound);
    }

    // inclusive
    template <typename T>
    T
    get360_angular(const range<T>& range, float value)
    {
        const float half_difference
                = (range.upper_bound - range.lower_bound) / 2.0f;
        return range.lower_bound + half_difference
               - half_difference * cos(angle::from_percent_of_360(value));
    }

    template <typename T, typename F>
    T
    remap_to(const range<F>& from, const range<T>& to, F value)
    {
        return from01(to, to01(from, value));
    }

    // includsive, both min and max are included
    template <typename T>
    bool
    is_within(const range<T>& range, T value)
    {
        return value >= range.lower_bound && value <= range.upper_bound;
    }

    // inclusive, both min and max are included
    template <typename T>
    T
    keep_within(const range<T>& range, T value)
    {
        if(value > range.upper_bound)
        {
            return range.upper_bound;
        }
        if(value < range.lower_bound)
        {
            return range.lower_bound;
        }

        return value;
    }

    template <typename T>
    T
    wrap(const range<T>& range, T value)
    {
        const T diff = range.upper_bound - range.lower_bound;
        ASSERT(diff > 0);
        T wrapped = value - range.lower_bound;
        while(wrapped < 0)
        {
            wrapped += diff;
        }
        while(wrapped > diff)
        {
            wrapped -= diff;
        }
        return range.lower_bound + wrapped;
    }

}

