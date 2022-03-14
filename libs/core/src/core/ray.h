#pragma once

#include "core/vec3.h"
#include "core/mat4.h"

namespace euphoria::core
{
    struct UnitRay3f
    {
        UnitRay3f(const Vec3f& from, const Unit3f& dir);

        [[nodiscard]] static UnitRay3f
        from_to(const Vec3f& from, const Vec3f& to);

        [[nodiscard]] static UnitRay3f
        from_direction(const Unit3f& dir);

        [[nodiscard]] Vec3f
        get_point(float at) const;

        [[nodiscard]] UnitRay3f
        get_transform(const Mat4f& m) const;

        Vec3f from;
        Unit3f dir;
    };


    struct Ray2f
    {
        Ray2f(const Vec2f& p, const Vec2f& d);

        [[nodiscard]] static Ray2f
        from_to(const Vec2f& from, const Vec2f& to);

        [[nodiscard]] static Ray2f
        from_direction(const Vec2f& direction, const Vec2f& pos = Vec2f::zero());

        [[nodiscard]] Vec2f
        get_position(float d) const;

        Vec2f position;
        Vec2f direction;
    };

    struct Ray3f
    {
        Ray3f(const Vec3f& from, const Vec3f& dir);

        [[nodiscard]] static Ray3f
        from_to(const Vec3f& from, const Vec3f& to);

        [[nodiscard]] static Ray3f
        from_direction(const Vec3f& dir);

        [[nodiscard]] Vec3f
        get_point(float at) const;

        [[nodiscard]] UnitRay3f
        get_normalized() const;

        Vec3f from;
        Vec3f dir;
    };

}
