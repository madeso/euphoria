#include "core/camera3.h"

#include "core/assert.h"

namespace euphoria::core
{
    compiled_camera3::compiled_camera3(const mat4f& view_, const mat4f& projection_)
        : view(view_)
        , projection(projection_)
        , combined(view_ * projection_)
        , combined_inverted(projection_ * view_)
    {
        const bool was_inverted = combined_inverted.invert();
        ASSERT(was_inverted);
    }

    vec3f
    compiled_camera3::world_to_clip(const vec3f& in_world) const
    {
        const vec4f v = combined * vec4f {in_world, 1};

        // divide by w to get back to the 1.0f coordinate space
        return vec3f {v.x, v.y, v.z} / v.w;
    }

    vec3f
    compiled_camera3::clip_to_world(const vec3f& in_clip) const
    {
        const vec4f v = combined_inverted * vec4f {in_clip, 1};

        // divide by w to get back to the 1.0f coordinate space
        return vec3f {v.x, v.y, v.z} / v.w;
    }

    ray3f
    compiled_camera3::clip_to_world_ray(const vec2f& p) const
    {
        const auto from = clip_to_world(vec3f {p, -1.0f});
        const auto to = clip_to_world(vec3f {p, 1.0f});
        return ray3f::from_to(from, to);
    }

    camera3::camera3()
        : position(vec3f::zero())
        , rotation(quatf::identity())
        , fov(45.0f)
        , near(0.1f)
        , far(100.0f)
    {}

    namespace
    {
        mat4f
        CalculateProjectionMatrix(const camera3& camera, float aspect)
        {
            const mat4f projection_matrix = mat4f::create_perspective(
                    angle::from_degrees(camera.fov),
                    aspect,
                    camera.near,
                    camera.far);
            return projection_matrix;
        }

        mat4f
        CalculateViewMatrix(const camera3& camera)
        {
            return camera.rotation.get_conjugate().to_mat4()
                   * mat4f::from_translation(
                           -static_cast<vec3f>(camera.position));
        }
    } // namespace

    compiled_camera3
    camera3::compile(float aspect) const
    {
        return compiled_camera3 {CalculateViewMatrix(*this),
                               CalculateProjectionMatrix(*this, aspect)};
    }

} // namespace euphoria::core
