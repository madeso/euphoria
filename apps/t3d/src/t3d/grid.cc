#include "t3d/grid.h"

namespace euphoria::t3d
{
    float
    snap_to(float val, float step)
    {
        return core::round(val, step);
    }
}