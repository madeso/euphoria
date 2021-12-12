#include "render/viewport.h"

#include "core/cint.h"

#include "render/gl.h"

namespace euphoria::render
{
    viewport::viewport(const core::Recti& r)
        : bounds(r)
    {
    }


    void
    viewport::activate() const
    {
        const auto bottom_left = bounds.get_bottom_left();
        glViewport
        (
            bottom_left.x,
            bottom_left.y,
            bounds.get_width(),
            bounds.get_height()
        );

        // some gl funcs doesnt respect viewport, so we need to enable
        // scissor testing too
        glScissor
        (
            bottom_left.x,
            bottom_left.y,
            bounds.get_width(),
            bounds.get_height()
        );
    }


    float
    viewport::get_aspect_ratio() const
    {
        return core::c_int_to_float(bounds.get_width()) / core::c_int_to_float(bounds.get_height());
    }


    core::Vec2f
    viewport::to_clip_coord(const core::Vec2i& p) const
    {
        const auto p01 = core::to01(bounds.StaticCast<float>(), p.StaticCast<float>());
        const auto clip = core::from01(core::Rectf::from_left_right_top_bottom(-1, 1, 1, -1), p01);
        return clip;
    }
}
