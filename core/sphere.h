#ifndef EUPHORIA_CORE_SPHERE_H
#define EUPHORIA_CORE_SPHERE_H

#include "core/vec3.h"

namespace euphoria::core
{
    struct Sphere
    {
        vec3f center;
        float radius;
    };
}

#endif  // EUPHORIA_CORE_SPHERE_H
