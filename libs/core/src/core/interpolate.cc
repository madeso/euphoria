#include "core/interpolate.h"

namespace euphoria::core
{
    float
    FloatTransform::Transform(float from, float v, float to)
    {
        return from + (to - from) * v;
    }
}
