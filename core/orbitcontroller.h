#ifndef EUPHORIA_CORE_ORBITCONTROLLER_H
#define EUPHORIA_CORE_ORBITCONTROLLER_H

#include "core/vec3.h"
#include "core/quat.h"

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

        vec3f center;

        float distance;

        Angle horizontal_rotation;
        Angle vertical_rotation;
    };
}

#endif  // EUPHORIA_CORE_ORBITCONTROLLER_H
