#include "core/interpolate.default.h"

namespace euphoria::core
{
    float float_transform(float from, float v, float to)
    {
        return from + (to - from) * v;
    }
}


