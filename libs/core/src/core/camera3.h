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
        CompiledCamera3(const Mat4f& v, const Mat4f& p);

        [[nodiscard]]
        Vec3f
        world_to_clip(const Vec3f& in_world) const;

        [[nodiscard]]
        Vec3f
        clip_to_world(const Vec3f& in_clip) const;

        [[nodiscard]]
        Ray3f
        clip_to_world_ray(const Vec2f& p) const;

        Mat4f view;
        Mat4f projection;
        Mat4f combined;
        Mat4f combined_inverted;
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
