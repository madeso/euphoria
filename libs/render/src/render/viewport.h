#ifndef RENDER_VIEWPORT_H
#define RENDER_VIEWPORT_H

#include "core/rect.h"
#include "core/vec2.h"

namespace euphoria::render
{
    struct Viewport
    {
    public:
        Viewport(const core::Recti& viewport);

        void
        Activate() const;  // sets the gl viewport

        float
        GetAspectRatio() const;

        core::vec2f
        ToClipCoord(const core::vec2i& p) const;

        core::Recti viewport;
    };
}  // namespace euphoria::render

#endif  // RENDER_VIEWPORT_H
