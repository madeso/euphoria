#include "core/camera3.h"

#include "core/assert.h"

namespace euphoria::core
{
    compiled_camera3::compiled_camera3(const mat4f& v, const mat4f& p)
        : view(v)
        , projection(p)
        , combined(v * p)
        , combined_inverted((p * v).get_inverted())
    {
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
        calculate_projection_matrix(const camera3& camera, float aspect)
        {
            const mat4f projection_matrix = mat4f::create_perspective
            (
                angle::from_degrees(camera.fov),
                aspect,
                camera.near,
                camera.far
            );
            return projection_matrix;
        }

        mat4f
        calculate_view_matrix(const camera3& camera)
        {
            const auto rotation = camera.rotation.get_conjugate().to_mat4();
            const auto translation = mat4f::from_translation(-static_cast<vec3f>(camera.position));
            return rotation * translation;
        }
    }

    compiled_camera3
    camera3::compile(float aspect) const
    {
        return compiled_camera3 {calculate_view_matrix(*this), calculate_projection_matrix(*this, aspect)};
    }

}
