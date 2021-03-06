#pragma once


#include "core/vec3.h"
#include "core/quat.h"
#include "core/sensitivity.h"

namespace euphoria::core
{
    struct orbit_controller
    {
        orbit_controller();


        [[nodiscard]] quatf
        get_rotation() const;


        [[nodiscard]] vec3f
        get_camera_position() const;


        void
        on_pan_input(float dx, float dy);


        void
        on_rotate_input(float dx, float dy);


        void
        on_zoom_input(float z);


        vec3f center;

        angle horizontal_rotation;
        angle vertical_rotation;

        float distance = 10.0f;

        sensitivity pan_dx;
        sensitivity pan_dy;
        sensitivity rotate_dx;
        sensitivity rotate_dy;
        sensitivity zoom;
    };
}

