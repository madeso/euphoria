#include "core/numeric.h"

#include "core/assert.h"
#include "core/angle.h"
#include "core/range.h"

#include <cmath>


namespace euphoria::core
{
    bool
    is_equal(float lhs, float rhs)
    {
        return is_zero(lhs - rhs);
    }


    bool
    is_zero(float r)
    {
        const float epsilon = 0.0001f;
        return is_within(make_range(-epsilon, epsilon), r);
    }


    float
    zero_or_value(float r)
    {
        if(is_zero(r)) { return 0; }
        else { return r; }
    }


    float
    floor(float v)
    {
        return std::floor(v);
    }


    float
    ceil(float v)
    {
        return std::ceil(v);
    }


    int
    floor_to_int(float v)
    {
        return std::floor(v);
    }


    int
    ceil_to_int(float v)
    {
        return std::ceil(v);
    }


    int
    get_sign(float r)
    {
        if(r >= 0.0f) { return 1; }
        else { return -1; }
    }


    int
    get_sign(int r)
    {
        if(r >= 0) { return 1; }
        else { return -1; }
    }


    float
    get_sign(bool b)
    {
        if(b) { return  1.0f; }
        else  { return -1.0f; }
    }


    float
    lerp(float f, float scale, float t)
    {
        return f + (t - f) * scale;
    }


    float
    curve(float new_value, float old_value, float smoothing_value)
    {
        const int sign = get_sign(old_value - new_value);
        const float slip = (old_value - new_value) / smoothing_value;
        const float val = old_value - slip;

        if(sign != get_sign(val - new_value))
        {
            return new_value;
        }

        return val;
    }


    float
    square(float r)
    {
        return r * r;
    }


    float
    sqrt(float r)
    {
        ASSERTX(r >= 0 && "input must be bigger than 0", r);
        return std::sqrt(r);
    }


    float
    log(float r)
    {
        return std::log(r);
    }


    float
    abs(float r)
    {
        if(r < 0.0f)
        {
            return -r;
        }

        return r;
    }


    float
    min(float lhs, float rhs)
    {
        if(lhs < rhs)
        {
            return lhs;
        }

        return rhs;
    }


    int
    min(int lhs, int rhs)
    {
        if(lhs < rhs)
        {
            return lhs;
        }

        return rhs;
    }


    float
    max(float lhs, float rhs)
    {
        if(lhs > rhs)
        {
            return lhs;
        }

        return rhs;
    }


    int
    max(int lhs, int rhs)
    {
        if(lhs > rhs)
        {
            return lhs;
        }

        return rhs;
    }


    float
    mod(float numer, float denumer)
    {
        return ::fmodf(numer, denumer);
    }


    bool
    is_within_inclusive_as_int(int min, int c, int max)
    {
        return c >= min && c <= max;
    }


    namespace // internal
    {
        float
        get_lower_bound(float num, float gran)
        {
            return std::floor(num / gran) * gran;
        }


        float
        get_upper_bound(float num, float gran)
        {
            return std::ceil(num / gran) * gran;
        }
    }


    float
    round(float num, float gran)
    {
        const float lower = get_lower_bound(num, gran);
        const float upper = get_upper_bound(num, gran);
        const float percent
                = (num - lower)
                  / gran; // 1.0 is upper, 0.0 is lower, 0.25 is 25% between
        // lower and upper, moving from lower to upper

        if(percent >= 0.5)
        {
            return upper;
        }

        return lower;
    }
}
