#pragma once

#include <optional>

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
    get_intersection(const UnitRay3f& r, const Aabb& aabb);

    float
    get_intersection(const UnitRay3f& r, const Plane& p);



    // --------------------------------
    // ray - ray

    Ray2Ray2Result
    get_intersection(const Ray2f& lhs, const Ray2f& rhs);



    // --------------------------------
    // plane - point

    float
    get_distance_between(const Plane& plane, const Vec3f& p);

    Vec3f
    get_closest_point(const Plane& plane, const Vec3f& p);



    // --------------------------------
    // ray - point

    float
    get_distance_between(const UnitRay3f& ray, const Vec3f& p);

    Vec3f
    get_closest_point(const UnitRay3f& ray, const Vec3f& c);



    // --------------------------------
    // sphere - sphere

    bool
    get_intersection(const Sphere& lhs, const Vec3f& lhs_center, const Sphere& rhs, const Vec3f& rhs_center);



    // --------------------------------
    // sphere - point

    bool
    contains_point(const Sphere& sphere, const Vec3f& sphere_center, const Vec3f& point);

    Vec3f
    get_closest_point(const Sphere& sphere, const Vec3f& sphere_center, const Vec3f& point);



    // --------------------------------
    // sphere - ray

    // returns distance along the ray where intersection occured or -1 if nothing occured
    float
    get_intersection(const UnitRay3f& r, const Sphere& sphere, const Vec3f& sphere_center);



    // --------------------------------
    // aabb - point

    bool
    contains_point(const Aabb& aabb, const Vec3f& point);

    Vec3f
    get_closest_point(const Aabb& aabb, const Vec3f& point);



    // --------------------------------
    // point inside 2d geoms

    bool
    is_point_in_triangle(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& p);




    // --------------------------------
    // 3d ray to triangle

    std::optional<float>
    get_intersection_ray_triangle
    (
        const UnitRay3f& ray,
        const Vec3f& v0, const Vec3f& v1, const Vec3f& v2
    );
    



    ///////////////////////////////////////////////////////////////////////////
    // collision results

    // todo(Gustav): replace with std::optional?
    struct Ray2Ray2Result
    {
        bool collision;
        bool is_parallel;

        Vec2f point;
        float u;
        float v;
    };

    // todo(Gustav): replace with std::optional?
    struct Ray3AabbResult
    {
        bool intersected;

        float start;
        float end;
    };

}

