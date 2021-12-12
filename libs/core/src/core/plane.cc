#include "core/plane.h"

namespace euphoria::core
{
    Plane::Plane(const Unit3f& n, float d)
        : normal(n)
        , distance(d)
    {}


    Plane
    Plane::from_normal_and_point(const Unit3f& normal, const Vec3f& point)
    {
        return {normal, -dot(normal, point)};
    }
    

    Plane
    Plane::from_points(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2)
    {
        return from_normal_and_point(cross(p1-p0, p2-p0).get_normalized(), p0);
    }
}
