#include "core/plane.h"

namespace euphoria::core
{
    Plane::Plane(const unit3f& n, float d)
        : normal(n)
        , distance(d)
    {}


    Plane
    Plane::from_normal_and_point(const unit3f& normal, const vec3f& point)
    {
        return {normal, -dot(normal, point)};
    }
    

    Plane
    Plane::from_points(const vec3f& p0, const vec3f& p1, const vec3f& p2)
    {
        return from_normal_and_point(cross(p1-p0, p2-p0).get_normalized(), p0);
    }
}
