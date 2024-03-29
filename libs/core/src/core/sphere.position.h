#pragma once

#include "base/vec3.h"
#include "core/sphere.h"


namespace eu::core
{
    struct SphereAndPosition
    {
        Sphere sphere;
        vec3f center;
    };
}

