#include "core/intersection.h"

#include "core/ray.h"
#include "core/aabb.h"
#include "core/sphere.h"
#include "core/plane.h"

namespace euphoria::core
{
    namespace
    {
        ray3_aabb_result
        Ray3AabbResult_False()
        {
            auto r = ray3_aabb_result{};
            r.intersected = false;
            r.start = r.end = -1.0f;
            return r;
        }


        ray3_aabb_result
        Ray3AabbResult_True(float start, float end)
        {
            auto r = ray3_aabb_result{};
            r.intersected = true;
            r.start = start;
            r.end = end;
            return r;
        }
    }


    ray3_aabb_result
    get_intersection
    (
        const unit_ray3f& r,
        const aabb& aabb
    )
    {
        // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection

        // todo(Gustav): refactor aabb class?
        const vec3f bounds[] {aabb.min, aabb.max};

        // todo(Gustav): move to ray class?
        const vec3f r_invdir = 1.0f / static_cast<vec3f>(r.dir);
        const int r_sign[3] =
        {
            r_invdir.x < 0 ? 1 : 0,
            r_invdir.y < 0 ? 1 : 0,
            r_invdir.z < 0 ? 1 : 0
        };

        float tmin = (bounds[r_sign[0]].x - r.from.x) * r_invdir.x;
        float tmax = (bounds[1 - r_sign[0]].x - r.from.x) * r_invdir.x;
        const float tymin = (bounds[r_sign[1]].y - r.from.y) * r_invdir.y;
        const float tymax = (bounds[1 - r_sign[1]].y - r.from.y) * r_invdir.y;

        if((tmin > tymax) || (tymin > tmax))
        {
            return Ray3AabbResult_False();
        }
        if(tymin > tmin)
        {
            tmin = tymin;
        }
        if(tymax < tmax)
        {
            tmax = tymax;
        }

        const float tzmin = (bounds[r_sign[2]].z - r.from.z) * r_invdir.z;
        const float tzmax = (bounds[1 - r_sign[2]].z - r.from.z) * r_invdir.z;

        if((tmin > tzmax) || (tzmin > tmax))
        {
            return Ray3AabbResult_False();
        }

        if(tzmin > tmin)
        {
            tmin = tzmin;
        }
        if(tzmax < tmax)
        {
            tmax = tzmax;
        }

        return Ray3AabbResult_True(tmin, tmax);
    }


    float
    get_intersection
    (
        const unit_ray3f& r,
        const plane& p
    )
    {
        return -(dot(r.from, p.normal) + p.distance)/dot(r.dir, p.normal);
    }


    namespace
    {
        ray2_ray2_result
        Ray2Ray2Result_Parallel()
        {
            return {false, true, vec2f::zero(), -1.0f, -1.0f};
        }

        ray2_ray2_result
        Ray2Ray2Result_NoCollision()
        {
            return {false, false, vec2f::zero(), -1.0f, -1.0f};
        }

        ray2_ray2_result
        Ray2Ray2Result_Collided(const vec2f& p, float a, float b)
        {
            return {false, true, p, a, b};
        }
    }


    ray2_ray2_result
    get_intersection
    (
        const ray2f& lhs,
        const ray2f& rhs
    )
    {
        // https://stackoverflow.com/a/1968345/180307
        const vec2f p1 = lhs.position;
        const vec2f p2 = lhs.position + lhs.direction;
        const vec2f p3 = rhs.position;
        const vec2f p4 = rhs.position + rhs.direction;

        const float p0_x = p1.x;
        const float p0_y = p1.y;
        const float p1_x = p2.x;
        const float p1_y = p2.y;
        const float p2_x = p3.x;
        const float p2_y = p3.y;
        const float p3_x = p4.x;
        const float p3_y = p4.y;

        const float s1_x = p1_x - p0_x;
        const float s1_y = p1_y - p0_y;
        const float s2_x = p3_x - p2_x;
        const float s2_y = p3_y - p2_y;

        const float den = (-s2_x * s1_y + s1_x * s2_y);

        // todo(Gustav): implement a check for zero for float
        if(abs(den) < 0.00001f)
        {
            return Ray2Ray2Result_Parallel();
        }

        const float s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / den;
        const float t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / den;

        if(s >= 0 && s <= 1 && t >= 0 && t <= 1)
        {
            const float x = p0_x + (t * s1_x);
            const float y = p0_y + (t * s1_y);
            return Ray2Ray2Result_Collided(vec2f(x, y), s, t);
        }

        return Ray2Ray2Result_NoCollision();
    }

    float
    get_distance_between
    (
        const plane& plane,
        const vec3f& p
    )
    {
        return dot(plane.normal, p) + plane.distance;
    }

    vec3f
    get_closest_point
    (
        const plane& plane,
        const vec3f& point
    )
    {
        const auto distance = dot(plane.normal, point) - plane.distance;
        return point - distance * plane.normal;
    }


    float
    get_distance_between
    (
        const unit_ray3f& ray,
        const vec3f& point
    )
    {
        const auto newNorm = (point - ray.from).get_normalized();

        const auto d = dot(newNorm, ray.dir);
        return abs(1.0f - d);
    }

    vec3f
    get_closest_point
    (
        const unit_ray3f& ray,
        const vec3f& c
    )
    {
        const auto ab = ray;
        const auto a = ray.from;
        // const auto b = ray.GetPoint(1);

        auto t = dot(c - a, ab.dir) / dot(ab.dir, ab.dir);

        t = max(t, 0.0f);

        const auto d = ray.get_point(t);
        return d;
    }


    bool
    get_intersection
    (
        const sphere& lhs,
        const vec3f& lhs_center,
        const sphere& rhs,
        const vec3f& rhs_center
    )
    {
        return vec3f::from_to(lhs_center, rhs_center).get_length_squared() < square(lhs.radius + rhs.radius);
    }


    bool
    contains_point
    (
        const sphere& sphere,
        const vec3f& sphere_center,
        const vec3f& point
    )
    {
        return vec3f::from_to(sphere_center, point).get_length_squared() < square(sphere.radius);
    }


    vec3f
    get_closest_point
    (
        const sphere& sphere,
        const vec3f& sphere_center,
        const vec3f& point
    )
    {
        return unit_ray3f::from_to(sphere_center, point).get_point(sphere.radius);
    }


    float
    get_intersection
    (
        const unit_ray3f& ray,
        const sphere& sphere,
        const vec3f& sphere_center
    )
    {
        const vec3f oc = ray.from - sphere_center;
        const auto a = dot(ray.dir, ray.dir);
        const auto b = 2.0f * dot(oc, ray.dir);
        const auto c = dot(oc, oc) - sphere.radius*sphere.radius;
        const auto discriminant = b*b - 4*a*c;
        if (discriminant < 0) {
            return -1.0f;
        }
        else {
            return (-b - sqrt(discriminant) ) / (2.0f*a);
        }
    }


    bool
    contains_point
    (
        const aabb& aabb,
        const vec3f& point
    )
    {
        ASSERT(aabb.is_valid());

        return
            // greater than min
            point.x >= aabb.min.x &&
            point.y >= aabb.min.y &&
            point.z >= aabb.min.z &&
            // and less than max
            point.x <= aabb.max.x &&
            point.y <= aabb.max.y &&
            point.z <= aabb.max.z;
    }


    vec3f
    get_closest_point
    (
        const aabb& aabb,
        const vec3f& point
    )
    {
        ASSERT(aabb.is_valid());

        #define VEC(N) (point.N > aabb.max.N ? aabb.max.N : (point.N < aabb.min.N ? aabb.min.N : point.N))
        return {VEC(x), VEC(y), VEC(z)};
        #undef VEC
    }


    bool
    is_point_in_triangle
    (
        const vec2f& A,
        const vec2f& B,
        const vec2f& C,
        const vec2f& P
    )
    {
        const auto s1 = C.y - A.y;
        const auto s2 = C.x - A.x;
        const auto s3 = B.y - A.y;
        const auto s4 = P.y - A.y;

        const auto w1 = (A.x*s1 + s4*s2 - P.x*s1) / (s3*s2 - (B.x - A.x)*s1);
        const auto w2 = (s4 - w1*s3) / s1;
        return w1 >= 0 && w2 >= 0 && (w1 + w2) <= 1;
    }


}
