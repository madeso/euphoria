#ifndef EUPHORIA_INTERSECTION_H
#define EUPHORIA_INTERSECTION_H

#include "core/vec2.h"
#include "core/vec3.h"

namespace euphoria::core
{
    struct Ray2f;
    struct UnitRay3f;
    struct Aabb;
    struct Sphere;

    struct RayIntersectionResult
    {
        bool  intersected;
        float start;
        float end;
    };

    RayIntersectionResult
    Intersect(const UnitRay3f& r, const Aabb& aabb);

    struct Ray2fIntersectionResult
    {
        [[nodiscard]] static Ray2fIntersectionResult
        Parallel();

        [[nodiscard]] static Ray2fIntersectionResult
        NoCollision();

        [[nodiscard]] static Ray2fIntersectionResult
        Collided(const vec2f& p, float a, float b);

        [[nodiscard]] static Ray2fIntersectionResult
        GetClosestCollision(const Ray2fIntersectionResult& a, const Ray2fIntersectionResult& b);

        bool collision;
        bool is_parallel;

        vec2f point;
        float      u;
        float      v;

    private:
        Ray2fIntersectionResult();
        Ray2fIntersectionResult(const vec2f& p, float a, float b);
    };

    Ray2fIntersectionResult
    GetIntersection(const Ray2f& lhs, const Ray2f& rhs);

    bool
    GetIntersection(const Sphere& lhs, const Sphere& rhs);

    bool
    ContainsPoint(const Sphere& sphere, const vec3f& point);

    vec3f
    ClosestPoint(const Sphere& sphere, const vec3f& point);

}  // namespace euphoria::core

#endif  // EUPHORIA_INTERSECTION_H
