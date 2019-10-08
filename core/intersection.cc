#include "core/intersection.h"

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

}  // namespace euphoria::core
