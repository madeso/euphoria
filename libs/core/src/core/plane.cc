#include "core/plane.h"

namespace euphoria::core
{
    plane::plane(const unit3f& n, float d)
        : normal(n)
        , distance(d)
    {}


    plane
    plane::from_normal_and_point(const unit3f& normal, const vec3f& point)
    {
        return {normal, -dot(normal, point)};
    }
    

    plane
    plane::from_points(const vec3f& p0, const vec3f& p1, const vec3f& p2)
    {
        return from_normal_and_point(cross(p1-p0, p2-p0).get_normalized(), p0);
    }
}
