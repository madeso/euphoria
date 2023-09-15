#pragma once



#include "core/vec3.h"
#include "core/sphere.position.h"


namespace eu::core
{
    SphereAndPosition
    build_bounding_sphere(const std::vector<vec3f>& positions);
}

