#pragma once

#include "core/vec3.h"

namespace eu::core
{
    struct Plane
    {
        unit3f normal;
        float distance;

        Plane(const unit3f& n, float d);

        static Plane
        from_normal_and_point(const unit3f& normal, const vec3f& point);

        static Plane
        from_points(const vec3f& p0, const vec3f& p1, const vec3f& p2);
    };
}
