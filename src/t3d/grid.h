#ifndef EUPHORIA_T3D_GRID_H
#define EUPHORIA_T3D_GRID_H

namespace euphoria::t3d
{
    struct Grid
    {
        bool visible = true;
        float small_step = 0.5f;
        int big_step_interval = 5;
        float normal = 1.0f;
        int size = 10;

        bool snap_enabled = true;
    };
}

#endif  // EUPHORIA_T3D_GRID_H
