#include "core/interpolate.default.h"

namespace euphoria::core
{
    // todo(Gustav): replace with lerp
    // also refactor standard lerp to lerp_float
    float float_transform(float from, float v, float to)
    {
        return from + (to - from) * v;
    }
}


