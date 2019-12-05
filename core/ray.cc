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
