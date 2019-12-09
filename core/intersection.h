#ifndef EUPHORIA_INTERSECTION_H
#define EUPHORIA_INTERSECTION_H

#include "core/vec2.h"
#include "core/vec3.h"

namespace euphoria::core
{
    // defined elsewhere
    struct Ray2f;
    struct Ray3f;
    struct UnitRay3f;
    struct Aabb;
    struct Sphere;
    struct Plane;

    // defined later
    struct Ray3AabbResult;
    struct Ray2Ray2Result;


    ///////////////////////////////////////////////////////////////////////////
    // collision functions

    // --------------------------------
    // ray - aabb

    Ray3AabbResult
    GetIntersection(const UnitRay3f& r, const Aabb& aabb);


    // --------------------------------
    // ray - ray

    Ray2Ray2Result
    GetIntersection(const Ray2f& lhs, const Ray2f& rhs);


    // --------------------------------
    // plane - point

    float
    DistanceBetween(const Plane& plane, const vec3f& p);

    vec3f
    ClosestPoint(const Plane& plane, const vec3f& p);


    // --------------------------------
    // ray - point

    float
    DistanceBetween(const UnitRay3f& ray, const vec3f& p);

    vec3f
    ClosestPoint(const UnitRay3f& ray, const vec3f& p);


    // --------------------------------
    // sphere - sphere

    bool
    GetIntersection(const Sphere& lhs, const vec3f& lhs_center, const Sphere& rhs, const vec3f& rhs_center);


    // --------------------------------
    // sphere - point

    bool
    ContainsPoint(const Sphere& sphere, const vec3f& sphere_center, const vec3f& point);

    vec3f
    ClosestPoint(const Sphere& sphere, const vec3f& sphere_center, const vec3f& point);


    // --------------------------------
    // sphere - ray

    // returns distance along the ray where intersection occured or -1 if nothing occured
    float
    GetIntersection(const UnitRay3f& r, const Sphere& sphere, const vec3f& sphere_center);


    // --------------------------------
    // aabb - point

    bool
    ContainsPoint(const Aabb& aabb, const vec3f& point);

    vec3f
    ClosestPoint(const Aabb& aabb, const vec3f& point);


    ///////////////////////////////////////////////////////////////////////////
    // collision results

    struct Ray2Ray2Result
    {
        bool collision;
        bool is_parallel;

        vec2f point;
        float      u;
        float      v;
    };

    struct Ray3AabbResult
    {
        bool  intersected;

        float start;
        float end;
    };

}  // namespace euphoria::core

#endif  // EUPHORIA_INTERSECTION_H
