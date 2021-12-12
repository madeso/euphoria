#pragma once

#include "core/mat4.h"
#include "core/vec2.h"
#include "core/vec3.h"
#include "core/quat.h"
#include "core/ray.h"


namespace euphoria::core
{
    struct CompiledCamera3
    {
    public:
        CompiledCamera3(const mat4f& v, const mat4f& p);

        [[nodiscard]]
        Vec3f
        world_to_clip(const Vec3f& in_world) const;

        [[nodiscard]]
        Vec3f
        clip_to_world(const Vec3f& in_clip) const;

        [[nodiscard]]
        Ray3f
        clip_to_world_ray(const Vec2f& p) const;

        mat4f view;
        mat4f projection;
        mat4f combined;
        mat4f combined_inverted;
    };


    struct Camera3
    {
        Camera3();

        [[nodiscard]]
        CompiledCamera3
        compile(float aspect) const;

        Vec3f position;
        Quatf rotation;
        float fov;
        float near;
        float far;
    };
}
