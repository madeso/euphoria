#ifndef EUPHORIA_CORE_PLANE_H
#define EUPHORIA_CORE_PLANE_H

#include "core/vec3.h"

namespace euphoria::core
{
    struct plane
    {
        unit3f normal;
        float distance;

        plane(const unit3f& n, float d);

        static plane
        from_normal_and_point(const unit3f& normal, const vec3f& point);

        static plane
        from_points(const vec3f& p0, const vec3f& p1, const vec3f& p2);
    };
}

#endif  // EUPHORIA_CORE_PLANE_H
