#pragma once

#include "core/interpolate.default.h"


namespace euphoria::core
{

    // int overloads provided for template overload reasons


    bool
    is_equal(float lhs, float rhs);

    bool
    is_equal(int lhs, int rhs);

    bool
    is_zero(float r);

    bool
    is_zero(int r);

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


    int
    get_sign(int r);


    /** Returns either 1 or -1 */
    float
    get_sign(bool b);


    float
    lerp_float(float f, float scale, float t);

    DEFAULT_INTERPOLATE(float, lerp_float);

    /** Performs a single interpolating step to transform a old value to a new value.
    Larger smoothing values result in a smoother but less resposive path.

    Example:
    @code mousex = Curve( ( MousePos.X - OldMousePos.X ) * Sensitivity(), mousex, 6
    ); @endcode

    @param new_value is the target value
    @param old_value is the current value
    @param smoothing_value is the smoothing value > 0, the lower, the less smooth

    @returns The smoothed result.
    */
    float
    curve(float new_value, float old_value, float smoothing_value);

    float
    square(float r);

    float
    sqrt(float r);

    float
    log(float r);

    float
    abs(float r);

    template<typename T>
    constexpr T
    min(T lhs, T rhs)
    {
        if (lhs < rhs)
        {
            return lhs;
        }

        return rhs;
    }


    template<typename T>
    constexpr T
    max(T lhs, T rhs)
    {
        if (lhs > rhs)
        {
            return lhs;
        }

        return rhs;
    }

    float
    mod(float numer, float denumer);

    bool
    is_within_inclusive_as_int(int min, int c, int max);

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

}

