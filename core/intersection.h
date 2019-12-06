#ifndef EUPHORIA_INTERSECTION_H
#define EUPHORIA_INTERSECTION_H

#include "core/vec2.h"
#include "core/vec3.h"

namespace euphoria::core
{
    // defined elsewhere
    struct Ray2f;
    struct UnitRay3f;
    struct Aabb;
    struct Sphere;

    // defined later
    struct RayIntersectionResult;
    struct Ray2fIntersectionResult;


    ///////////////////////////////////////////////////////////////////////////
    // collision functions

    // --------------------------------
    // ray - rect

    RayIntersectionResult
    Intersect(const UnitRay3f& r, const Aabb& aabb);

    // --------------------------------
    // ray - ray

    Ray2fIntersectionResult
    GetIntersection(const Ray2f& lhs, const Ray2f& rhs);


    // --------------------------------
    // sphere - sphere

    bool
    GetIntersection(const Sphere& lhs, const Sphere& rhs);


    // --------------------------------
    // sphere - point

    bool
    ContainsPoint(const Sphere& sphere, const vec3f& point);

    vec3f
    ClosestPoint(const Sphere& sphere, const vec3f& point);


    ///////////////////////////////////////////////////////////////////////////
    // collision results

    struct Ray2fIntersectionResult
    {
        bool collision;
        bool is_parallel;

        vec2f point;
        float      u;
        float      v;
    };

    struct RayIntersectionResult
    {
        bool  intersected;

        float start;
        float end;
    };

}  // namespace euphoria::core

#endif  // EUPHORIA_INTERSECTION_H
