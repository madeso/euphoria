#pragma once


#include "core/vec3.h"
#include "core/quat.h"
#include "core/sensitivity.h"

namespace euphoria::core
{
    struct OrbitController
    {
        vec3f center;
        Angle horizontal_rotation;
        Angle vertical_rotation;
        float distance = 10.0f;
        Sensitivity pan_dx;
        Sensitivity pan_dy;
        Sensitivity rotate_dx;
        Sensitivity rotate_dy;
        Sensitivity zoom;

        OrbitController();

        void on_pan_input(float dx, float dy);
        void on_rotate_input(float dx, float dy);
        void on_zoom_input(float z);

        [[nodiscard]] quatf get_rotation() const;
        [[nodiscard]] vec3f get_camera_position() const;
    };
}

