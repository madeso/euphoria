#include "core/ray.h"



namespace euphoria::core
{


    UnitRay3f::UnitRay3f(const vec3f& f, const unit3f& d)
        : from(f)
        , dir(d)
    {
    }


    [[nodiscard]] UnitRay3f
    UnitRay3f::from_to(const vec3f& afrom, const vec3f& ato)
    {
        return UnitRay3f{afrom, vec3f::from_to(afrom, ato).get_normalized()};
    }


    [[nodiscard]] UnitRay3f
    UnitRay3f::from_direction(const unit3f& adir)
    {
        return UnitRay3f{zero3f, adir};
    }


    vec3f
    UnitRay3f::get_point(float at) const
    {
        return from + dir * at;
    }


    [[nodiscard]] UnitRay3f
    UnitRay3f::get_transform(const mat4f& m) const
    {
        return
        {
            m.get_transform_point(from),
            m.get_transform_vec(dir)
        };
    }

    
    [[nodiscard]] Ray2f
    Ray2f::from_direction(const vec2f& direction, const vec2f& pos)
    {
        return Ray2f(pos, direction);
    }


    [[nodiscard]] Ray2f
    Ray2f::from_to(const vec2f& from, const vec2f& to)
    {
        return Ray2f(from, vec2f::from_to(from, to));
    }


    vec2f
    Ray2f::get_position(float d) const
    {
        return position + d * direction;
    }
    

    Ray2f::Ray2f(const vec2f& p, const vec2f& d) : position(p), direction(d) {}

    
    Ray3f::Ray3f(const vec3f& f, const vec3f& d)
        : from(f)
        , dir(d)
    {
    }


    [[nodiscard]] Ray3f
    Ray3f::from_to(const vec3f& from, const vec3f& to)
    {
        return Ray3f{from, vec3f::from_to(from, to)};
    }


    [[nodiscard]] Ray3f
    Ray3f::from_direction(const vec3f& dir)
    {
        return Ray3f{zero3f, dir};
    }


    vec3f
    Ray3f::get_point(float at) const
    {
        return from + dir * at;
    }

    UnitRay3f
    Ray3f::get_normalized() const
    {
        return UnitRay3f{from, dir.get_normalized()};
    }


}

