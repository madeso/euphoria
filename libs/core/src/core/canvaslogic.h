#pragma once

#include "core/vec2.h"
#include "core/range.h"

namespace euphoria::core
{
    struct CanvasLogic
    {
        // canvas view
        Vec2f scroll = Vec2f {0, 0};
        float scale = 1;

        // "config"
        Range<float> scale_range = make_range(0.1f, 15.0f);

        void
        pan(const Vec2f& p);

        void
        zoom(const Vec2f& mouse, float zoom);

        [[nodiscard]] Vec2f
        screen_to_world(const Vec2f& p) const;

        [[nodiscard]] Vec2f
        world_to_screen(const Vec2f& p) const;
    };
}
