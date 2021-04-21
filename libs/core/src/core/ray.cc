#include "core/ray.h"

namespace euphoria::core
{
    unit_ray3f::unit_ray3f(const vec3f& from, const unit3f& dir) : from(from), dir(dir) {}

    [[nodiscard]] unit_ray3f
    unit_ray3f::from_to(const vec3f& from, const vec3f& to)
    {
        return unit_ray3f{from, vec3f::from_to(from, to).get_normalized()};
    }

    [[nodiscard]] unit_ray3f
    unit_ray3f::from_direction(const unit3f& dir)
    {
        return unit_ray3f{vec3f::zero(), dir};
    }

    vec3f
    unit_ray3f::get_point(float at) const
    {
        return from + dir * at;
    }

    
    [[nodiscard]] ray2f
    ray2f::from_direction(const vec2f& direction, const vec2f& pos)
    {
        return ray2f(pos, direction);
    }

    [[nodiscard]] ray2f
    ray2f::from_to(const vec2f& from, const vec2f& to)
    {
        return ray2f(from, vec2f::from_to(from, to));
    }

    vec2f
    ray2f::get_position(float d) const
    {
        return position + d * direction;
    }
    
    ray2f::ray2f(const vec2f& p, const vec2f& d) : position(p), direction(d) {}


    
    ray3f::ray3f(const vec3f& from, const vec3f& dir) : from(from), dir(dir) {}

    [[nodiscard]] ray3f
    ray3f::from_to(const vec3f& from, const vec3f& to)
    {
        return ray3f{from, vec3f::from_to(from, to)};
    }

    [[nodiscard]] ray3f
    ray3f::from_direction(const vec3f& dir)
    {
        return ray3f{vec3f::zero(), dir};
    }

    vec3f
    ray3f::get_point(float at) const
    {
        return from + dir * at;
    }

    unit_ray3f
    ray3f::get_normalized() const
    {
        return unit_ray3f{from, dir.get_normalized()};
    }
}
