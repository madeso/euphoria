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

        vec3f
        WorldToClip(const vec3f& in_world) const;

        vec3f
        ClipToWorld(const vec3f& in_clip) const;

        Ray3f
        ClipToWorldRay(const vec2f& p) const;

        mat4f view;
        mat4f projection;
        mat4f combined;
        mat4f combined_inverted;
    };


    struct Camera
    {
    public:
        Camera();

        CompiledCamera
        Compile(float aspect) const;

        mat4f
        CalculateProjectionMatrix(float aspect) const;

        mat4f
        CalculateViewMatrix() const;

        vec3f position;
        quatf rotation;
        float fov;
        float near;
        float far;
    };
}  // namespace euphoria::core

#endif  // EUPHORIA_CAMERA_H
