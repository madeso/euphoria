#ifndef EUPHORIA_CORE_PLANE_H
#define EUPHORIA_CORE_PLANE_H

#include "core/vec3.h"

namespace euphoria::core
{
    struct Plane
    {
        unit3f normal;
        float distance;

        Plane(const unit3f& n, float d);

        static Plane
        FromNormalAndPoint(const unit3f& normal, const vec3f& point);

        static Plane
        FromPoints(const vec3f& p0, const vec3f& p1, const vec3f& p2);
    };
}

#endif  // EUPHORIA_CORE_PLANE_H
