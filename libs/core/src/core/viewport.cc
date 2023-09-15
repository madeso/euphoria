#include "core/viewport.h"

#include "core/cint.h"
#include "core/camera3.h"

namespace eu::core
{
    Viewport::Viewport(const Recti& r)
        : bounds(r)
    {
    }


    float
    Viewport::get_aspect_ratio() const
    {
        return c_int_to_float(bounds.get_width()) / c_int_to_float(bounds.get_height());
    }


    vec2f
    Viewport::to_clip_coord(const vec2i& p) const
    {
        const auto p01 = to01(bounds.to_f(), p.to_f());
        const auto clip = from_01(Rectf::from_left_right_top_bottom(-1, 1, 1, -1), p01);
        return clip;
    }


    core::Ray3f from_mouse_to_ray
    (
        const core::CompiledCamera3& camera,
        const core::Viewport& viewport,
        const core::vec2i& position
    )
    {
        return camera.from_clip_to_world_ray
        (
            viewport.to_clip_coord
            (
                position
            )
        );
    }


    core::UnitRay3f from_mouse_to_unit_ray
    (
        const core::CompiledCamera3& camera,
        const core::Viewport& viewport,
        const core::vec2i& position
    )
    {
        const auto ray = from_mouse_to_ray(camera, viewport, position);
        return ray.get_normalized();
    }
}
