#pragma once

#include "core/vec2.h"
#include "core/range.h"

namespace euphoria::core
{
    struct CanvasLogic
    {
        // canvas view
        vec2f scroll = vec2f {0, 0};
        float scale = 1;

        // "config"
        Range<float> scale_range = make_range(0.1f, 15.0f);

        void
        pan(const vec2f& p);

        void
        zoom(const vec2f& mouse, float zoom);

        [[nodiscard]] vec2f
        screen_to_world(const vec2f& p) const;

        [[nodiscard]] vec2f
        world_to_screen(const vec2f& p) const;
    };
}
