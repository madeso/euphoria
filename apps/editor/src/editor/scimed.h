#pragma once

#include <memory>

#include "core/vec2.h"

#include "window/canvas.h"

#include "gaf_scalingsprite.h"


namespace euphoria::render
{
    struct texture2d;
    struct texture_cache;
}


namespace euphoria::editor
{
    struct line_hover_data
    {
        int vertical_index = -1;
        int horizontal_index = -1;
    };

    struct scimed_config
    {
        int sizer_distance = 20;
        int sizer_text_distance = 23;
        int anchor_size = 6;
        unsigned int sizer_color = IM_COL32(0, 0, 0, 255);
        unsigned int split_color = IM_COL32(0, 255, 0, 255);
    };

    struct scimed
    {
        std::shared_ptr<scalingsprite::ScalingSprite> scaling;

        std::shared_ptr<render::texture2d> texture;
        window::canvas canvas;
        core::Vec2i mouse_popup = core::Vec2i {0, 0};
        line_hover_data hover;

        bool
        run(const window::canvas_config& cc, const scimed_config& scc);
    };
}
