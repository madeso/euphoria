#ifndef EUPHORIA_INTERSECTION_H
#define EUPHORIA_INTERSECTION_H

namespace euphoria::core
{
    struct UnitRay3f;
    struct Aabb;

    struct RayIntersectionResult
    {
        bool  intersected;
        float start;
        float end;
    };

    RayIntersectionResult
    Intersect(const UnitRay3f& r, const Aabb& aabb);

}  // namespace euphoria::core

#endif  // EUPHORIA_INTERSECTION_H
