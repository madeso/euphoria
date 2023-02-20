#pragma once

#include "core/vec3.h"
#include "core/mat4.h"

namespace euphoria::core
{
    struct UnitRay3f
    {
        vec3f from;
        unit3f dir;

        UnitRay3f(const vec3f& from, const unit3f& dir);

        [[nodiscard]] static UnitRay3f from_to(const vec3f& from, const vec3f& to);
        [[nodiscard]] static UnitRay3f from_direction(const unit3f& dir);

        [[nodiscard]] vec3f get_point(float at) const;
        [[nodiscard]] UnitRay3f get_transform(const mat4f& m) const;
    };


    struct Ray2f
    {
        vec2f position;
        vec2f direction;

        Ray2f(const vec2f& p, const vec2f& d);

        [[nodiscard]] static Ray2f from_to(const vec2f& from, const vec2f& to);
        [[nodiscard]] static Ray2f from_direction(const vec2f& direction, const vec2f& pos = vec2f::zero());

        [[nodiscard]] vec2f get_position(float d) const;
    };

    struct Ray3f
    {
        vec3f from;
        vec3f dir;

        Ray3f(const vec3f& from, const vec3f& dir);

        [[nodiscard]] static Ray3f from_to(const vec3f& from, const vec3f& to);
        [[nodiscard]] static Ray3f from_direction(const vec3f& dir);

        [[nodiscard]] vec3f get_point(float at) const;
        [[nodiscard]] UnitRay3f get_normalized() const;
    };

}
