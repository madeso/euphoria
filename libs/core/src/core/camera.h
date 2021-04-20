#ifndef EUPHORIA_CAMERA_H
#define EUPHORIA_CAMERA_H

#include "core/mat4.h"
#include "core/vec2.h"
#include "core/vec3.h"
#include "core/quat.h"
#include "core/ray.h"


namespace euphoria::core
{
    struct CompiledCamera
    {
    public:
        CompiledCamera(const mat4f& view_, const mat4f& projection_);

        [[nodiscard]]
        vec3f
        WorldToClip(const vec3f& in_world) const;

        [[nodiscard]]
        vec3f
        ClipToWorld(const vec3f& in_clip) const;

        [[nodiscard]]
        ray3f
        ClipToWorldRay(const vec2f& p) const;

        mat4f view;
        mat4f projection;
        mat4f combined;
        mat4f combined_inverted;
    };


    struct Camera
    {
        Camera();

        [[nodiscard]]
        CompiledCamera
        Compile(float aspect) const;

        vec3f position;
        quatf rotation;
        float fov;
        float near;
        float far;
    };
}  // namespace euphoria::core

#endif  // EUPHORIA_CAMERA_H
