#include "render/viewport.h"

#include "render/gl.h"

namespace euphoria::render
{
    Viewport::Viewport(const core::Recti& viewport) : viewport(viewport) {}

    void
    Viewport::Activate() const
    {
        const auto bottom_left = viewport.BottomLeft();
        glViewport(
                bottom_left.x,
                bottom_left.y,
                viewport.GetWidth(),
                viewport.GetHeight());

        // some gl funcs doesnt respect viewport, so we need to enable
        // scissor testing too
        glScissor(
                bottom_left.x,
                bottom_left.y,
                viewport.GetWidth(),
                viewport.GetHeight());
    }

    float
    Viewport::GetAspectRatio() const
    {
        return viewport.GetWidth() / static_cast<float>(viewport.GetHeight());
    }


    core::vec2f
    Viewport::ToClipCoord(const core::vec2i& p) const
    {
        auto p01 = core::To01(viewport.StaticCast<float>(), p.StaticCast<float>());
        auto clip = core::From01(core::Rectf::FromLeftRightTopBottom(-1, 1, 1, -1), p01);
        return clip;
    }

}  // namespace euphoria::render
