#pragma once

#include "core/mat4.h"
#include "core/vec2.h"
#include "core/vec3.h"
#include "core/quat.h"
#include "core/ray.h"


namespace euphoria::core
{
    struct compiled_camera3
    {
    public:
        compiled_camera3(const mat4f& view_, const mat4f& projection_);

        [[nodiscard]]
        vec3f
        world_to_clip(const vec3f& in_world) const;

        [[nodiscard]]
        vec3f
        clip_to_world(const vec3f& in_clip) const;

        [[nodiscard]]
        ray3f
        clip_to_world_ray(const vec2f& p) const;

        mat4f view;
        mat4f projection;
        mat4f combined;
        mat4f combined_inverted;
    };


    struct camera3
    {
        camera3();

        [[nodiscard]]
        compiled_camera3
        compile(float aspect) const;

        vec3f position;
        quatf rotation;
        float fov;
        float near;
        float far;
    };
}
