#pragma once


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
    struct ray3_aabb_result;
    struct ray2_ray2_result;


    ///////////////////////////////////////////////////////////////////////////
    // collision functions

    // --------------------------------
    // ray - aabb

    ray3_aabb_result
    get_intersection(const unit_ray3f& r, const aabb& aabb);

    float
    get_intersection(const unit_ray3f& r, const plane& p);



    // --------------------------------
    // ray - ray

    ray2_ray2_result
    get_intersection(const ray2f& lhs, const ray2f& rhs);



    // --------------------------------
    // plane - point

    float
    get_distance_between(const plane& plane, const vec3f& p);

    vec3f
    get_closest_point(const plane& plane, const vec3f& p);



    // --------------------------------
    // ray - point

    float
    get_distance_between(const unit_ray3f& ray, const vec3f& p);

    vec3f
    get_closest_point(const unit_ray3f& ray, const vec3f& c);



    // --------------------------------
    // sphere - sphere

    bool
    get_intersection(const sphere& lhs, const vec3f& lhs_center, const sphere& rhs, const vec3f& rhs_center);



    // --------------------------------
    // sphere - point

    bool
    contains_point(const sphere& sphere, const vec3f& sphere_center, const vec3f& point);

    vec3f
    get_closest_point(const sphere& sphere, const vec3f& sphere_center, const vec3f& point);



    // --------------------------------
    // sphere - ray

    // returns distance along the ray where intersection occured or -1 if nothing occured
    float
    get_intersection(const unit_ray3f& r, const sphere& sphere, const vec3f& sphere_center);



    // --------------------------------
    // aabb - point

    bool
    contains_point(const aabb& aabb, const vec3f& point);

    vec3f
    get_closest_point(const aabb& aabb, const vec3f& point);



    // --------------------------------
    // point inside 2d geoms

    bool
    is_point_in_triangle(const vec2f& A, const vec2f& B, const vec2f& C, const vec2f& P);



    ///////////////////////////////////////////////////////////////////////////
    // collision results

    struct ray2_ray2_result
    {
        bool collision;
        bool is_parallel;

        vec2f point;
        float      u;
        float      v;
    };

    struct ray3_aabb_result
    {
        bool  intersected;

        float start;
        float end;
    };

}

