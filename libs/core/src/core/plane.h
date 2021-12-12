#pragma once

#include "core/vec3.h"

namespace euphoria::core
{
    struct Plane
    {
        Unit3f normal;
        float distance;

        Plane(const Unit3f& n, float d);

        static Plane
        from_normal_and_point(const Unit3f& normal, const Vec3f& point);

        static Plane
        from_points(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2);
    };
}
