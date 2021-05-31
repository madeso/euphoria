#pragma once

#include "core/rect.h"
#include "core/vec2.h"

namespace euphoria::render
{
    struct viewport
    {
        viewport(const core::recti& r);

        /** Sets the gl viewport.
         */
        void
        activate() const;

        [[nodiscard]] float
        get_aspect_ratio() const;

        [[nodiscard]] core::vec2f
        to_clip_coord(const core::vec2i& p) const;

        core::recti bounds;
    };
}
