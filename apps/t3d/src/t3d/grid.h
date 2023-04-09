#pragma once

#include "core/angle.h"

namespace euphoria::t3d
{
    struct Grid
    {
        bool visible = true;
        float small_step = 0.5f;
        int big_step_interval = 5;
        float normal = 1.0f;
        int size = 10;

        core::Angle angle_snap = core::Angle::from_percent_of_360(5.0f / 60.0f);

        bool snap_enabled = true;
    };

    float
    snap_to(float val, float step);
}

