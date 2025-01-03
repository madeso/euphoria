#pragma once

#include "base/ints.h"


namespace eu
{
    /** \addtogroup math
     *  @{
     */

    constexpr float abs(float r)
    {
        return r >= 0.0f
            ? r
            : -r
            ;
    }


    constexpr bool is_zero(float r)
    {
        constexpr float epsilon = 0.0001f;
        return abs(r) < epsilon;
    }

    constexpr bool
    is_equal(float lhs, float rhs)
    {
        return is_zero(lhs - rhs);
    }

    float
    clamp_zero(float r);

    float
    floor(float v);

    float
    ceil(float v);

    int
    floor_to_int(float v);

    int
    ceil_to_int(float v);


    /** Calculates the sign as a positive or a negative int.
    @returns 1 if r is greater than 0, -1 if not.
    @see Abs()
    */
    int
    get_sign(float r);


    /// Returns `1` if `true` or -1
    float
    get_sign(bool b);


    float
    lerp_float(float f, float scale, float t);

    /// Return `r * r`
    float
    square(float r);

    float
    sqrt(float r);


    constexpr float min(float lhs, float rhs) { return lhs < rhs ? lhs : rhs; }
    constexpr float max(float lhs, float rhs) { return lhs > rhs ? lhs : rhs; }


    float
    mod(float numer, float denumer);

    /** Rounds a value to the nearest nice value.
        If the granularity is 1 the function rounds to the closest integer, at .5 the
        closest half integer, at 2 even integers etc...
        @param num the number to round
        @param gran the granularity
        */
    float
    round(float num, float gran);

    constexpr float pi = 3.1415926535897932384626433832795f;
    // constexpr float half_pi = pi / 2.0f;

    /// If the `value` is close to zero, `def` is returned
    template <typename T>
    T
    get_default_if_close_to_zero(T value, T def, T epsilon)
    {
        if(abs(value) < epsilon) { return def; }
        else { return value; }
    }

    /// returns true for a index each Nth loop. pass zero to always return false
    /// useful for example adding a newline each 10th or 15th line
    template <typename I>
    constexpr bool
    is_each_nth(I i, I each)
    {
        return each && (i % each) == each - 1;
    }

    /** @}*/
}

