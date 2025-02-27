#include "base/numeric.h"

#include "assert/assert.h"
#include "base/angle.h"

#include <cmath>


namespace eu
{
    float
    clamp_zero(float r)
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
        return static_cast<int>(std::floor(v));
    }


    int
    ceil_to_int(float v)
    {
        return static_cast<int>(std::ceil(v));
    }


    int
    get_sign(float r)
    {
        if(r >= 0.0f) { return 1; }
        else { return -1; }
    }


    float
    get_sign(bool b)
    {
        if(b) { return  1.0f; }
        else  { return -1.0f; }
    }


    float
    lerp_float(float f, float scale, float t)
    {
        return f + (t - f) * scale;
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
    mod(float numer, float denumer)
    {
        return ::fmodf(numer, denumer);
    }


    float
    round(float num, float gran)
    {
        const float lower = std::floor(num / gran) * gran;
        const float upper = std::ceil(num / gran) * gran;

        const float percent = (num - lower) / gran;
        // 1.0 is upper, 0.0 is lower
        // 0.25 is 25% between lower and upper

        if(percent >= 0.5f)
        {
            return upper;
        }

        return lower;
    }
}
