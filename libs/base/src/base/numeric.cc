#include "base/numeric.h"

#include "assert/assert.h"
#include "base/angle.h"

#include <cmath>


namespace eu
{
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
}
