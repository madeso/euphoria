#include "core/interpolate.h"

namespace euphoria::core
{
    float
    FloatTransform::transform(float from, float v, float to)
    {
        return from + (to - from) * v;
    }
}
