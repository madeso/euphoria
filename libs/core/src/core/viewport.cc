#include "core/viewport.h"

#include "core/cint.h"
#include "core/camera3.h"

namespace euphoria::core
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


    Vec2f
    Viewport::to_clip_coord(const Vec2i& p) const
    {
        const auto p01 = to01(static_cast<Rectf>(bounds), static_cast<Vec2f>(p));
        const auto clip = from01(Rectf::from_left_right_top_bottom(-1, 1, 1, -1), p01);
        return clip;
    }


    core::Ray3f mouse_to_ray
    (
        const core::CompiledCamera3& camera,
        const core::Viewport& viewport,
        const core::Vec2i& position
    )
    {
        return camera.clip_to_world_ray
        (
            viewport.to_clip_coord
            (
                position
            )
        );
    }


    core::UnitRay3f mouse_to_unit_ray
    (
        const core::CompiledCamera3& camera,
        const core::Viewport& viewport,
        const core::Vec2i& position
    )
    {
        return mouse_to_ray(camera, viewport, position).get_normalized();
    }
}
