#pragma once



#include <type_traits>


#include "base/angle.h"
#include "assert/assert.h"

namespace eu
{
    template <typename T>
    struct Range
    {
        T lower_bound;
        T upper_bound;

        constexpr Range(T min, T max) : lower_bound(min), upper_bound(max)
        {
            ASSERTX(lower_bound <= upper_bound, upper_bound, lower_bound);
        }

        explicit Range(T max) : lower_bound(0), upper_bound(max)
        {
            ASSERTX(lower_bound <= upper_bound, upper_bound, lower_bound);
        }

        [[nodiscard]] T
        get_distance() const
        {
            return upper_bound - lower_bound;
        }
    };


    template <typename T>
    Range<T> make_range(T min, T max)
    {
        return {min, max};
    }

    template <typename T>
    Range<T> make_range(T max)
    {
        return Range<T>(max);
    }

    constexpr Range<float> r01 = { 0.0f, 1.0f};
    constexpr Range<float> r11 = { -1.0f, 1.0};

    float from_01f(float lower_bound, float upper_bound, float value);

    template <typename T>
    T from_01(const Range<T>& range, float value)
    {
        const float r = from_01f
        (
            static_cast<float>(range.lower_bound),
            static_cast<float>(range.upper_bound),
            value
        );

        if constexpr (std::is_unsigned<T>::value)
        {
            ASSERT(r >= 0.0f);
        }

        return static_cast<T>(r);
    }

    template <>
    float from_01(const Range<float>& range, float value);

    template <typename T>
    float to01(const Range<T>& range, T value)
    {
        return (value - range.lower_bound)
               / (range.upper_bound - range.lower_bound);
    }

    // inclusive
    template <typename T>
    T get360_angular(const Range<T>& range, float value)
    {
        const float half_difference
                = (range.upper_bound - range.lower_bound) / 2.0f;
        return range.lower_bound + half_difference
               - half_difference * cos(Angle::from_percent_of_360(value));
    }

    template <typename T, typename F>
    T remap_to(const Range<F>& from, const Range<T>& to, F value)
    {
        return from_01(to, to01(from, value));
    }

    // includsive, both min and max are included
    template <typename T>
    bool is_within(const Range<T>& range, T value)
    {
        return value >= range.lower_bound && value <= range.upper_bound;
    }

    // inclusive, both min and max are included
    template <typename T>
    T keep_within(const Range<T>& range, T value)
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
    T wrap(const Range<T>& range, T value)
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

