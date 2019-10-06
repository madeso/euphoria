#ifndef EUPHORIA_INTERSECTION_H
#define EUPHORIA_INTERSECTION_H

#include "core/ray.h"
#include "core/aabb.h"

namespace euphoria::core
{
    struct RayIntersectionResult
    {
        bool  intersected;
        float start;
        float end;
    };

    RayIntersectionResult
    Intersect(const UnitRay3f& r, const Aabb& aabb);

}  // namespace euphoria::core

#endif  // EUPHORIA_INTERSECTION_H
