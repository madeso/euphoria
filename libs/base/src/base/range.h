#pragma once

#include <type_traits>

#include "base/angle.h"
#include "assert/assert.h"


namespace eu
{
    /** \addtogroup math
     *  @{
     */

    /// A (inclusive) range between two values.
    /// Zero range is allowed.
    template <typename T>
    struct R
    {
        T lower_bound;
        T upper_bound;

        /// Asserts if max is below min
        constexpr R(T min, T max) : lower_bound(min), upper_bound(max)
        {
            ASSERTX(lower_bound <= upper_bound, upper_bound, lower_bound);
        }

        [[nodiscard]] T
        get_distance() const
        {
            return upper_bound - lower_bound;
        }
    };


    /// Create a range from `min` to `max` (inclusive)
    template <typename T>
    R<T> make_range(T min, T max)
    {
        return {min, max};
    }

    /// Create a range from zero (0) to `max` (inclusive)
    template <typename T>
    R<T> make_range(T max)
    {
        return R<T>(0, max);
    }

    /// A range going from 0 to 1
    constexpr R<float> r01 = { 0.0f, 1.0f};

    // A range going from -1 to 1
    constexpr R<float> r11 = { -1.0f, 1.0};

    float from_01f(float lower_bound, float upper_bound, float value);

    /// Converts a value in 0-1 range to a custom range
    template <typename T>
    T from_01(const R<T>& range, float value)
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
    float from_01(const R<float>& range, float value);

    /// Converts a value in a range to the 0-1 range
    template <typename T>
    float to01(const R<T>& range, T value)
    {
        return (value - range.lower_bound)
               / (range.upper_bound - range.lower_bound);
    }

    // inclusive
    template <typename T>
    T get360_angular(const R<T>& range, float value)
    {
        const float half_difference
                = (range.upper_bound - range.lower_bound) / 2.0f;
        return range.lower_bound + half_difference
               - half_difference * cos(An::from_percent_of_360(value));
    }

    /// Remaps a value from one range to another
    template <typename T, typename F>
    T remap_to(const R<F>& from, const R<T>& to, F value)
    {
        return from_01(to, to01(from, value));
    }

    /// Returns true if a value is withing a range
    template <typename T>
    bool is_within(const R<T>& range, T value)
    {
        return value >= range.lower_bound && value <= range.upper_bound;
    }

    /// Returns a value that is kept within the range.
    /// i.e the value can't go lower than the lower bound or higher than the higher bound
    /// @see wrap
    template <typename T>
    T keep_within(const R<T>& range, T value)
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

    /// Returns a value that wraps around the range.
    /// When the value go outside the range, the value is wrapped back to either the start if exited at the end or vice versa.
    template <typename T>
    T wrap(const R<T>& range, T value)
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

    /** @}*/
}

