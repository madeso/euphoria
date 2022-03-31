#include "render/viewport.h"

#include "core/viewport.h"

#include "render/gl.h"


namespace euphoria::render
{
    void
    activate(const core::Viewport& vp)
    {
        const auto bottom_left = vp.bounds.get_bottom_left();
        glViewport
        (
            bottom_left.x,
            bottom_left.y,
            vp.bounds.get_width(),
            vp.bounds.get_height()
        );

        // some gl funcs doesnt respect viewport, so we need to enable
        // scissor testing too
        glScissor
        (
            bottom_left.x,
            bottom_left.y,
            vp.bounds.get_width(),
            vp.bounds.get_height()
        );
    }
}
