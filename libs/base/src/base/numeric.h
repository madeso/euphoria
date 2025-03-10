#pragma once

#include "base/ints.h"


namespace eu
{
    namespace kk
    {
        constexpr float epsilon = 0.0001f;
    }

    constexpr float pi = 3.1415926535897932384626433832795f;
    // constexpr float half_pi = pi / 2.0f;

    constexpr float cabs(float r)
    {
        return r >= 0.0f
            ? r
            : -r
            ;
    }

    /** \addtogroup math
     *  @{
     */
    constexpr bool is_zero(float r, float epsilon = kk::epsilon)
    {
        return cabs(r) < epsilon;
    }

    constexpr bool
    is_equal(float lhs, float rhs, float epsilon = kk::epsilon)
    {
        return is_zero(lhs - rhs, epsilon);
    }

    int
    floor_to_int(float v);

    int
    ceil_to_int(float v);


    /** Calculates the sign as a positive or a negative int.
    @returns 1 if r is greater than 0, -1 if not.
    */
    constexpr int
    get_sign(float r)
    {
        if(r >= 0.0f) { return 1; }
        else { return -1; }
    }


    /// Returns `1` if `true` or -1
    constexpr float
    get_sign(bool b)
    {
        if(b) { return  1.0f; }
        else  { return -1.0f; }
    }


    constexpr float
    lerp_float(float f, float scale, float t)
    {
        return f + (t - f) * scale;
    }

    /// Return `r * r`
    constexpr float
    square(float r)
    {
        return r * r;
    }

    constexpr float min(float lhs, float rhs) { return lhs < rhs ? lhs : rhs; }
    constexpr float max(float lhs, float rhs) { return lhs > rhs ? lhs : rhs; }
    

    /// If the `value` is close to zero, `def` is returned
    constexpr float
    clamp_zero(float value, float def = 0.0f, float epsilon = kk::epsilon)
    {
        if(is_zero(value, epsilon)) { return def; }
        else { return value; }
    }

    /** @}*/
}

