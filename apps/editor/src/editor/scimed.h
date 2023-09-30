#pragma once

#include <memory>

#include "base/vec2.h"

#include "window/canvas.h"

#include "files/scalingsprite.h"


namespace eu::render
{
    struct Texture2;
    struct TextureCache;
}


namespace eu::editor
{
    struct LineHoverData
    {
        int vertical_index = -1;
        int horizontal_index = -1;
    };

    struct ScimedConfig
    {
        int sizer_distance = 20;
        int sizer_text_distance = 23;
        int anchor_size = 6;
        unsigned int sizer_color = IM_COL32(0, 0, 0, 255);
        unsigned int split_color = IM_COL32(0, 255, 0, 255);
    };

    // SCalable IMage EDitor
    struct Scimed
    {
        std::shared_ptr<files::scalingsprite::ScalingSprite> scaling;

        std::shared_ptr<render::Texture2> texture;
        window::Canvas canvas;
        vec2i mouse_popup = vec2i{0, 0};
        LineHoverData hover;

        bool
        run(const window::CanvasConfig& cc, const ScimedConfig& scc);
    };
}
