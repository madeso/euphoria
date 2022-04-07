#pragma once

#include <vector>

#include "core/vec3.h"
#include "core/sphere.position.h"


namespace euphoria::core
{
    SphereAndPosition
    build_bounding_sphere(const std::vector<Vec3f>& positions);
}

