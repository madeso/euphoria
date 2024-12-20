#include "t3d/grid.h"

namespace eu::t3d
{
    float
    snap_to(float val, float step)
    {
        return round(val, step);
    }
}
