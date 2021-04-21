#ifndef EUPHORIA_INTERSECTION_H
#define EUPHORIA_INTERSECTION_H

#include "core/vec2.h"
#include "core/vec3.h"

namespace euphoria::core
{
    // defined elsewhere
    struct ray2f;
    struct ray3f;
    struct unit_ray3f;
    struct aabb;
    struct sphere;
    struct plane;

    // defined later
    struct Ray3AabbResult;
    struct Ray2Ray2Result;


    ///////////////////////////////////////////////////////////////////////////
    // collision functions

    // --------------------------------
    // ray - aabb

    Ray3AabbResult
    GetIntersection(const unit_ray3f& r, const aabb& aabb);


    float
    GetIntersection(const unit_ray3f& r, const plane& p);


    // --------------------------------
    // ray - ray

    Ray2Ray2Result
    GetIntersection(const ray2f& lhs, const ray2f& rhs);


    // --------------------------------
    // plane - point

    float
    DistanceBetween(const plane& plane, const vec3f& p);

    vec3f
    ClosestPoint(const plane& plane, const vec3f& p);


    // --------------------------------
    // ray - point

    float
    DistanceBetween(const unit_ray3f& ray, const vec3f& p);

    vec3f
    ClosestPoint(const unit_ray3f& ray, const vec3f& p);


    // --------------------------------
    // sphere - sphere

    bool
    GetIntersection(const sphere& lhs, const vec3f& lhs_center, const sphere& rhs, const vec3f& rhs_center);


    // --------------------------------
    // sphere - point

    bool
    ContainsPoint(const sphere& sphere, const vec3f& sphere_center, const vec3f& point);

    vec3f
    ClosestPoint(const sphere& sphere, const vec3f& sphere_center, const vec3f& point);


    // --------------------------------
    // sphere - ray

    // returns distance along the ray where intersection occured or -1 if nothing occured
    float
    GetIntersection(const unit_ray3f& r, const sphere& sphere, const vec3f& sphere_center);


    // --------------------------------
    // aabb - point

    bool
    ContainsPoint(const aabb& aabb, const vec3f& point);

    vec3f
    ClosestPoint(const aabb& aabb, const vec3f& point);


    // --------------------------------
    // point inside 2d geoms

    bool
    IsPointInTriangle(const vec2f& A, const vec2f& B, const vec2f& C, const vec2f& P);

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
