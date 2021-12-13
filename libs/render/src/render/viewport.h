#pragma once

#include "core/rect.h"
#include "core/vec2.h"

namespace euphoria::render
{
    struct Viewport
    {
        Viewport(const core::Recti& r);

        /** Sets the gl viewport.
         */
        void
        activate() const;

        [[nodiscard]] float
        get_aspect_ratio() const;

        [[nodiscard]] core::Vec2f
        to_clip_coord(const core::Vec2i& p) const;

        core::Recti bounds;
    };
}
