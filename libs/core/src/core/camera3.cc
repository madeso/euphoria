#include "core/camera3.h"

#include "core/assert.h"

namespace euphoria::core
{
    CompiledCamera3::CompiledCamera3(const mat4f& v, const mat4f& p)
        : view(v)
        , projection(p)
        , combined(v * p)
        , combined_inverted((p * v).get_inverted())
    {
    }

    Vec3f
    CompiledCamera3::world_to_clip(const Vec3f& in_world) const
    {
        const Vec4f v = combined * Vec4f {in_world, 1};

        // divide by w to get back to the 1.0f coordinate space
        return Vec3f {v.x, v.y, v.z} / v.w;
    }

    Vec3f
    CompiledCamera3::clip_to_world(const Vec3f& in_clip) const
    {
        const Vec4f v = combined_inverted * Vec4f {in_clip, 1};

        // divide by w to get back to the 1.0f coordinate space
        return Vec3f {v.x, v.y, v.z} / v.w;
    }

    Ray3f
    CompiledCamera3::clip_to_world_ray(const Vec2f& p) const
    {
        const auto from = clip_to_world(Vec3f {p, -1.0f});
        const auto to = clip_to_world(Vec3f {p, 1.0f});
        return Ray3f::from_to(from, to);
    }

    Camera3::Camera3()
        : position(Vec3f::zero())
        , rotation(Quatf::identity())
        , fov(45.0f)
        , near(0.1f)
        , far(100.0f)
    {}

    namespace
    {
        mat4f
        calculate_projection_matrix(const Camera3& camera, float aspect)
        {
            const mat4f projection_matrix = mat4f::create_perspective
            (
                Angle::from_degrees(camera.fov),
                aspect,
                camera.near,
                camera.far
            );
            return projection_matrix;
        }

        mat4f
        calculate_view_matrix(const Camera3& camera)
        {
            const auto rotation = camera.rotation.get_conjugate().to_mat4();
            const auto translation = mat4f::from_translation(-static_cast<Vec3f>(camera.position));
            return rotation * translation;
        }
    }

    CompiledCamera3
    Camera3::compile(float aspect) const
    {
        return CompiledCamera3 {calculate_view_matrix(*this), calculate_projection_matrix(*this, aspect)};
    }

}
