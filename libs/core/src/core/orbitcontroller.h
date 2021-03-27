#ifndef EUPHORIA_CORE_ORBITCONTROLLER_H
#define EUPHORIA_CORE_ORBITCONTROLLER_H

#include "core/vec3.h"
#include "core/quat.h"
#include "core/sensitivity.h"

namespace euphoria::core
{
    struct OrbitController
    {
        OrbitController();


        [[nodiscard]] quatf
        GetRotation() const;


        [[nodiscard]] vec3f
        GetCameraPosition() const;


        void
        Pan(float dx, float dy);


        void
        Rotate(float dx, float dy);


        void
        Zoom(float z);


        vec3f center;

        Angle horizontal_rotation;
        Angle vertical_rotation;

        float distance = 10.0f;

        Sensitivity pan_dx;
        Sensitivity pan_dy;
        Sensitivity rotate_dx;
        Sensitivity rotate_dy;
        Sensitivity zoom;
    };
}

#endif  // EUPHORIA_CORE_ORBITCONTROLLER_H
