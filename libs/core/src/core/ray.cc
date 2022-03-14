#include "core/ray.h"



namespace euphoria::core
{


    UnitRay3f::UnitRay3f(const Vec3f& f, const Unit3f& d)
        : from(f)
        , dir(d)
    {
    }


    [[nodiscard]] UnitRay3f
    UnitRay3f::from_to(const Vec3f& afrom, const Vec3f& ato)
    {
        return UnitRay3f{afrom, Vec3f::from_to(afrom, ato).get_normalized()};
    }


    [[nodiscard]] UnitRay3f
    UnitRay3f::from_direction(const Unit3f& adir)
    {
        return UnitRay3f{Vec3f::zero(), adir};
    }


    Vec3f
    UnitRay3f::get_point(float at) const
    {
        return from + dir * at;
    }


    [[nodiscard]] UnitRay3f
    UnitRay3f::get_transform(const Mat4f& m) const
    {
        return
        {
            m.get_transform_point(from),
            m.get_transform_vec(dir)
        };
    }

    
    [[nodiscard]] Ray2f
    Ray2f::from_direction(const Vec2f& direction, const Vec2f& pos)
    {
        return Ray2f(pos, direction);
    }


    [[nodiscard]] Ray2f
    Ray2f::from_to(const Vec2f& from, const Vec2f& to)
    {
        return Ray2f(from, Vec2f::from_to(from, to));
    }


    Vec2f
    Ray2f::get_position(float d) const
    {
        return position + d * direction;
    }
    

    Ray2f::Ray2f(const Vec2f& p, const Vec2f& d) : position(p), direction(d) {}

    
    Ray3f::Ray3f(const Vec3f& f, const Vec3f& d)
        : from(f)
        , dir(d)
    {
    }


    [[nodiscard]] Ray3f
    Ray3f::from_to(const Vec3f& from, const Vec3f& to)
    {
        return Ray3f{from, Vec3f::from_to(from, to)};
    }


    [[nodiscard]] Ray3f
    Ray3f::from_direction(const Vec3f& dir)
    {
        return Ray3f{Vec3f::zero(), dir};
    }


    Vec3f
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

