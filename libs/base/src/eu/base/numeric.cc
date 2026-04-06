#include "eu/base/numeric.h"

#include "eu/assert/assert.h"
#include "eu/base/angle.h"

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
