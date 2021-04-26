#pragma once

#include "core/rect.h"
#include "core/vec2.h"

namespace euphoria::render
{
    struct Viewport
    {
        Viewport(const core::recti& viewport);

        /** Sets the gl viewport.
         */
        void
        Activate() const;

        [[nodiscard]] float
        GetAspectRatio() const;

        [[nodiscard]] core::vec2f
        ToClipCoord(const core::vec2i& p) const;

        core::recti viewport;
    };
}
