#pragma once

#include "core/vec3.h"
#include "core/sphere.h"


namespace euphoria::core
{
    struct SphereAndPosition
    {
        Sphere sphere;
        vec3f center;
    };
}

