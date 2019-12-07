#include "core/plane.h"

namespace euphoria::core
{
    Plane::Plane(const unit3f& n, float d)
        : normal(n)
        , distance(d)
    {}


    Plane
    Plane::FromNormalAndPoint(const unit3f& normal, const vec3f& point)
    {
        return {normal, -dot(normal, point)};
    }
    

    Plane
    Plane::FromPoints(const vec3f& p0, const vec3f& p1, const vec3f& p2)
    {
        return FromNormalAndPoint(cross(p1-p0, p2-p0).GetNormalized(), p0);
    }
}
