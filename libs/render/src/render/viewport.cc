#include "render/viewport.h"

#include "core/cint.h"

#include "render/gl.h"

namespace euphoria::render
{
    Viewport::Viewport(const core::Recti& r)
        : bounds(r)
    {
    }


    void
    Viewport::activate() const
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
    Viewport::get_aspect_ratio() const
    {
        return core::c_int_to_float(bounds.get_width()) / core::c_int_to_float(bounds.get_height());
    }


    core::Vec2f
    Viewport::to_clip_coord(const core::Vec2i& p) const
    {
        const auto p01 = core::to01(static_cast<core::Rectf>(bounds), static_cast<core::Vec2f>(p));
        const auto clip = core::from01(core::Rectf::from_left_right_top_bottom(-1, 1, 1, -1), p01);
        return clip;
    }
}
