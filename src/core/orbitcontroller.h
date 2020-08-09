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


        quatf
        GetRotation() const;


        vec3f
        GetCameraPosition();


        void
        Pan(const float dx, const float dy);


        void
        Rotate(const float dx, const float dy);


        void
        Zoom(const float z);


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
