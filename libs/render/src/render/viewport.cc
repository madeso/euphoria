#include "render/viewport.h"

#include "render/gl.h"

namespace euphoria::render
{
    Viewport::Viewport(const core::recti& viewport)
        : viewport(viewport)
    {
    }


    void
    Viewport::Activate() const
    {
        const auto bottom_left = viewport.get_bottom_left();
        glViewport
        (
            bottom_left.x,
            bottom_left.y,
            viewport.get_width(),
            viewport.get_height()
        );

        // some gl funcs doesnt respect viewport, so we need to enable
        // scissor testing too
        glScissor
        (
            bottom_left.x,
            bottom_left.y,
            viewport.get_width(),
            viewport.get_height()
        );
    }


    float
    Viewport::GetAspectRatio() const
    {
        return viewport.get_width() / static_cast<float>(viewport.get_height());
    }


    core::vec2f
    Viewport::ToClipCoord(const core::vec2i& p) const
    {
        const auto p01 = core::to01(viewport.StaticCast<float>(), p.StaticCast<float>());
        const auto clip = core::from01(core::rectf::from_left_right_top_bottom(-1, 1, 1, -1), p01);
        return clip;
    }
}
