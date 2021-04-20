#include "core/camera.h"

#include "core/assert.h"

namespace euphoria::core
{
    CompiledCamera::CompiledCamera(const mat4f& view_, const mat4f& projection_)
        : view(view_)
        , projection(projection_)
        , combined(view_ * projection_)
        , combined_inverted(projection_ * view_)
    {
        const bool was_inverted = combined_inverted.Invert();
        ASSERT(was_inverted);
    }

    vec3f
    CompiledCamera::WorldToClip(const vec3f& in_world) const
    {
        const vec4f v = combined * vec4f {in_world, 1};

        // divide by w to get back to the 1.0f coordinate space
        return vec3f {v.x, v.y, v.z} / v.w;
    }

    vec3f
    CompiledCamera::ClipToWorld(const vec3f& in_clip) const
    {
        const vec4f v = combined_inverted * vec4f {in_clip, 1};

        // divide by w to get back to the 1.0f coordinate space
        return vec3f {v.x, v.y, v.z} / v.w;
    }

    Ray3f
    CompiledCamera::ClipToWorldRay(const vec2f& p) const
    {
        const auto from = ClipToWorld(vec3f {p, -1.0f});
        const auto to   = ClipToWorld(vec3f {p, 1.0f});
        return Ray3f::FromTo(from, to);
    }

    Camera::Camera()
        : position(vec3f::Zero())
        , rotation(quatf::Identity())
        , fov(45.0f)
        , near(0.1f)
        , far(100.0f)
    {}

    namespace
    {
        mat4f
        CalculateProjectionMatrix(const Camera& camera, float aspect)
        {
            const mat4f projection_matrix = mat4f::Perspective(
                    angle::from_degrees(camera.fov),
                    aspect,
                    camera.near,
                    camera.far);
            return projection_matrix;
        }

        mat4f
        CalculateViewMatrix(const Camera& camera)
        {
            return camera.rotation.GetConjugate().ToMat4()
                   * mat4f::FromTranslation(
                           -static_cast<vec3f>(camera.position));
        }
    }  // namespace

    CompiledCamera
    Camera::Compile(float aspect) const
    {
        return CompiledCamera {CalculateViewMatrix(*this),
                               CalculateProjectionMatrix(*this, aspect)};
    }

}  // namespace euphoria::core
