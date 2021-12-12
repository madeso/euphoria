#include "core/intersection.h"

#include "core/ray.h"
#include "core/aabb.h"
#include "core/sphere.h"
#include "core/plane.h"

namespace euphoria::core
{
    namespace
    {
        Ray3AabbResult
        ray3_aabb_result_false()
        {
            auto r = Ray3AabbResult{};
            r.intersected = false;
            r.start = r.end = -1.0f;
            return r;
        }


        Ray3AabbResult
        ray3_aabb_result_true(float start, float end)
        {
            auto r = Ray3AabbResult{};
            r.intersected = true;
            r.start = start;
            r.end = end;
            return r;
        }
    }


    Ray3AabbResult
    get_intersection
    (
        const UnitRay3f& r,
        const Aabb& aabb
    )
    {
        // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection

        // todo(Gustav): refactor aabb class?
        const Vec3f bounds[] {aabb.min, aabb.max};

        // todo(Gustav): move to ray class?
        const Vec3f r_invdir = 1.0f / static_cast<Vec3f>(r.dir);
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
            return ray3_aabb_result_false();
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
            return ray3_aabb_result_false();
        }

        if(tzmin > tmin)
        {
            tmin = tzmin;
        }
        if(tzmax < tmax)
        {
            tmax = tzmax;
        }

        return ray3_aabb_result_true(tmin, tmax);
    }


    float
    get_intersection
    (
        const UnitRay3f& r,
        const Plane& p
    )
    {
        return -(dot(r.from, p.normal) + p.distance)/dot(r.dir, p.normal);
    }


    namespace
    {
        Ray2Ray2Result
        ray2_ray2_result_parallel()
        {
            return {false, true, Vec2f::zero(), -1.0f, -1.0f};
        }

        Ray2Ray2Result
        ray2_ray2_result_no_collision()
        {
            return {false, false, Vec2f::zero(), -1.0f, -1.0f};
        }

        Ray2Ray2Result
        ray2_ray2_result_collided(const Vec2f& p, float a, float b)
        {
            return {false, true, p, a, b};
        }
    }


    Ray2Ray2Result
    get_intersection
    (
        const Ray2f& lhs,
        const Ray2f& rhs
    )
    {
        // https://stackoverflow.com/a/1968345/180307
        const Vec2f p1 = lhs.position;
        const Vec2f p2 = lhs.position + lhs.direction;
        const Vec2f p3 = rhs.position;
        const Vec2f p4 = rhs.position + rhs.direction;

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
            return ray2_ray2_result_parallel();
        }

        const float s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / den;
        const float t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / den;

        if(s >= 0 && s <= 1 && t >= 0 && t <= 1)
        {
            const float x = p0_x + (t * s1_x);
            const float y = p0_y + (t * s1_y);
            return ray2_ray2_result_collided(Vec2f(x, y), s, t);
        }

        return ray2_ray2_result_no_collision();
    }

    float
    get_distance_between
    (
        const Plane& plane,
        const Vec3f& p
    )
    {
        return dot(plane.normal, p) + plane.distance;
    }

    Vec3f
    get_closest_point
    (
        const Plane& plane,
        const Vec3f& point
    )
    {
        const auto distance = dot(plane.normal, point) - plane.distance;
        return point - distance * plane.normal;
    }


    float
    get_distance_between
    (
        const UnitRay3f& ray,
        const Vec3f& point
    )
    {
        const auto new_normalized = (point - ray.from).get_normalized();

        const auto d = dot(new_normalized, ray.dir);
        return abs(1.0f - d);
    }

    Vec3f
    get_closest_point
    (
        const UnitRay3f& ray,
        const Vec3f& c
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
        const Sphere& lhs,
        const Vec3f& lhs_center,
        const Sphere& rhs,
        const Vec3f& rhs_center
    )
    {
        return Vec3f::from_to(lhs_center, rhs_center).get_length_squared() < square(lhs.radius + rhs.radius);
    }


    bool
    contains_point
    (
        const Sphere& sphere,
        const Vec3f& sphere_center,
        const Vec3f& point
    )
    {
        return Vec3f::from_to(sphere_center, point).get_length_squared() < square(sphere.radius);
    }


    Vec3f
    get_closest_point
    (
        const Sphere& sphere,
        const Vec3f& sphere_center,
        const Vec3f& point
    )
    {
        return UnitRay3f::from_to(sphere_center, point).get_point(sphere.radius);
    }


    float
    get_intersection
    (
        const UnitRay3f& ray,
        const Sphere& sphere,
        const Vec3f& sphere_center
    )
    {
        const Vec3f oc = ray.from - sphere_center;
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
        const Aabb& aabb,
        const Vec3f& point
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


    Vec3f
    get_closest_point
    (
        const Aabb& aabb,
        const Vec3f& point
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
        const Vec2f& a,
        const Vec2f& b,
        const Vec2f& c,
        const Vec2f& p
    )
    {
        const auto s1 = c.y - a.y;
        const auto s2 = c.x - a.x;
        const auto s3 = b.y - a.y;
        const auto s4 = p.y - a.y;

        const auto w1 = (a.x * s1 + s4 * s2 - p.x * s1) / (s3 * s2 - (b.x - a.x) * s1);
        const auto w2 = (s4 - w1*s3) / s1;
        return w1 >= 0 && w2 >= 0 && (w1 + w2) <= 1;
    }


}
