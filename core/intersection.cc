#include "core/intersection.h"

#include "core/ray.h"
#include "core/aabb.h"

namespace euphoria::core
{
    RayIntersectionResult
    RayIntersectionResultFalse()
    {
        RayIntersectionResult r;
        r.intersected = false;
        r.start = r.end = -1.0f;
        return r;
    }

    RayIntersectionResult
    RayIntersectionResultTrue(float start, float end)
    {
        RayIntersectionResult r;
        r.intersected = true;
        r.start       = start;
        r.end         = end;
        return r;
    }

    // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
    RayIntersectionResult
    Intersect(const UnitRay3f& r, const Aabb& aabb)
    {
        // todo: refactor aabb class?
        const vec3f bounds[] {aabb.min, aabb.max};

        // todo: move to ray class?
        const vec3f r_invdir = 1.0f / static_cast<vec3f>(r.dir);
        const int r_sign[3]  = {r_invdir.x < 0, r_invdir.y < 0, r_invdir.z < 0};

        float       tmin  = (bounds[r_sign[0]].x - r.from.x) * r_invdir.x;
        float       tmax  = (bounds[1 - r_sign[0]].x - r.from.x) * r_invdir.x;
        const float tymin = (bounds[r_sign[1]].y - r.from.y) * r_invdir.y;
        const float tymax = (bounds[1 - r_sign[1]].y - r.from.y) * r_invdir.y;

        if((tmin > tymax) || (tymin > tmax))
        {
            return RayIntersectionResultFalse();
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
            return RayIntersectionResultFalse();
        }

        if(tzmin > tmin)
        {
            tmin = tzmin;
        }
        if(tzmax < tmax)
        {
            tmax = tzmax;
        }

        return RayIntersectionResultTrue(tmin, tmax);
    }


    
    [[nodiscard]] Ray2fIntersectionResult
    Ray2fIntersectionResult::Parallel()
    {
        Ray2fIntersectionResult c;
        c.is_parallel = true;
        return c;
    }

    [[nodiscard]] Ray2fIntersectionResult
    Ray2fIntersectionResult::NoCollision()
    {
        Ray2fIntersectionResult c;
        return c;
    }

    [[nodiscard]] Ray2fIntersectionResult
    Ray2fIntersectionResult::Collided(const vec2f& p, float a, float b)
    {
        Ray2fIntersectionResult c {p, a, b};
        return c;
    }

    [[nodiscard]] Ray2fIntersectionResult
    Ray2fIntersectionResult::GetClosestCollision(const Ray2fIntersectionResult& a, const Ray2fIntersectionResult& b)
    {
        if(a.collision && b.collision)
        {
            // determine closest
            if(a.u < b.u)
            {
                return a;
            }
            else
            {
                return b;
            }
        }
        else if(a.collision)
        {
            return a;
        }
        else if(b.collision)
        {
            return b;
        }
        else
        {
            return Ray2fIntersectionResult::NoCollision();
        }
    }

    Ray2fIntersectionResult::Ray2fIntersectionResult()
        : collision(false), is_parallel(false), point(0, 0), u(0), v(0)
    {}
    Ray2fIntersectionResult::Ray2fIntersectionResult(const vec2f& p, float a, float b)
        : collision(true), is_parallel(false), point(p), u(a), v(b)
    {}

    Ray2fIntersectionResult
    GetIntersection(const Ray2f& lhs, const Ray2f& rhs)
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

        // todo: implement a check for zero for float
        if(Abs(den) < 0.00001f)
        {
            return Ray2fIntersectionResult::Parallel();
        }

        const float s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / den;
        const float t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / den;

        if(s >= 0 && s <= 1 && t >= 0 && t <= 1)
        {
            const float x = p0_x + (t * s1_x);
            const float y = p0_y + (t * s1_y);
            return Ray2fIntersectionResult::Collided(vec2f(x, y), s, t);
        }

        return Ray2fIntersectionResult::NoCollision();
    }

}  // namespace euphoria::core
