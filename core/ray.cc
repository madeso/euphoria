#include "core/ray.h"

namespace euphoria::core
{
    UnitRay3f::UnitRay3f(const vec3f& from, const unit3f& dir) : from(from), dir(dir) {}

    [[nodiscard]] UnitRay3f
    UnitRay3f::FromTo(const vec3f& from, const vec3f& to)
    {
        return UnitRay3f{from, vec3f::FromTo(from, to).GetNormalized()};
    }

    [[nodiscard]] UnitRay3f
    UnitRay3f::FromDirection(const unit3f& dir)
    {
        return UnitRay3f{vec3f::Zero(), dir};
    }

    vec3f
    UnitRay3f::GetPoint(float at) const
    {
        return from + dir * at;
    }

    
    [[nodiscard]] Ray2f
    Ray2f::FromDirection(const vec2f& direction, const vec2f& pos)
    {
        return Ray2f(pos, direction);
    }

    [[nodiscard]] Ray2f
    Ray2f::FromTo(const vec2f& from, const vec2f& to)
    {
        return Ray2f(from, vec2f::FromTo(from, to));
    }

    vec2f
    Ray2f::GetPosition(float d) const
    {
        return position + d * direction;
    }
    
    Ray2f::Ray2f(const vec2f& p, const vec2f& d) : position(p), direction(d) {}


    
    Ray3f::Ray3f(const vec3f& from, const vec3f& dir) : from(from), dir(dir) {}

    [[nodiscard]] Ray3f
    Ray3f::FromTo(const vec3f& from, const vec3f& to)
    {
        return Ray3f{from, vec3f::FromTo(from, to)};
    }

    [[nodiscard]] Ray3f
    Ray3f::FromDirection(const vec3f& dir)
    {
        return Ray3f{vec3f::Zero(), dir};
    }

    vec3f
    Ray3f::GetPoint(float at) const
    {
        return from + dir * at;
    }

    UnitRay3f
    Ray3f::GetNormalized() const
    {
        return UnitRay3f{from, dir.GetNormalized()};
    }
}
