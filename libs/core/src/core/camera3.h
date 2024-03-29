#pragma once

#include "base/mat4.h"
#include "base/vec2.h"
#include "base/vec3.h"
#include "base/quat.h"
#include "base/ray.h"
#include "base/angle.h"


namespace eu::core
{
    struct CompiledCamera3
    {
        mat4f view;
        mat4f projection;
        mat4f combined;
        mat4f combined_inverted;

        CompiledCamera3(const mat4f& v, const mat4f& p);

        [[nodiscard]] vec3f from_world_to_clip(const vec3f& in_world) const;
        [[nodiscard]] vec3f from_clip_to_world(const vec3f& in_clip) const;
        [[nodiscard]] Ray3f from_clip_to_world_ray(const vec2f& p) const;
    };


    struct Camera3
    {
        vec3f position;
        quatf rotation;
        Angle fov;
        float near;
        float far;

        Camera3();

        [[nodiscard]] CompiledCamera3 compile(float aspect) const;
    };
}
