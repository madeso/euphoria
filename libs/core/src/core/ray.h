#pragma once

#include "core/vec3.h"

namespace euphoria::core
{
    struct unit_ray3f
    {
        unit_ray3f(const vec3f& from, const unit3f& dir);

        [[nodiscard]] static unit_ray3f
        from_to(const vec3f& from, const vec3f& to);

        [[nodiscard]] static unit_ray3f
        from_direction(const unit3f& dir);

        [[nodiscard]] vec3f
        get_point(float at) const;

        vec3f from;
        unit3f dir;
    };


    struct ray2f
    {
        ray2f(const vec2f& p, const vec2f& d);

        [[nodiscard]] static ray2f
        from_to(const vec2f& from, const vec2f& to);

        [[nodiscard]] static ray2f
        from_direction(const vec2f& direction, const vec2f& pos = vec2f::zero());

        [[nodiscard]] vec2f
        get_position(float d) const;

        vec2f position;
        vec2f direction;
    };

    struct ray3f
    {
        ray3f(const vec3f& from, const vec3f& dir);

        [[nodiscard]] static ray3f
        from_to(const vec3f& from, const vec3f& to);

        [[nodiscard]] static ray3f
        from_direction(const vec3f& dir);

        [[nodiscard]] vec3f
        get_point(float at) const;

        [[nodiscard]] unit_ray3f
        get_normalized() const;

        vec3f from;
        vec3f dir;
    };

}
