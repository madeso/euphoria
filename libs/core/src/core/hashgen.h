#pragma once

#include <optional>
#include <vector>

#include "core/rgb.h"
#include "core/ints.h"

namespace euphoria::core
{
    struct image;
}


namespace euphoria::core
{
    void render_retro(image* image, u32 code);
}

// converted to c++ from https://github.com/donpark/identicon/blob/master/identicon-canvas/identicon_canvas.js
namespace euphoria::core
{
    void render_identicon(image* image, u32 code);
}

// based on https://github.com/yurkth/sprator/
namespace euphoria::core
{
    void
    render_sprator
    (
        image* image,
        u32 code,
        const rgbai& foreground_color = {color::white},
        std::optional<rgbai> border_color_arg = std::nullopt,
        const rgbai& background_color = {color::black, 0}
    );

    void
    render_sprator
    (
        std::vector<image>* images,
        u32 code,
        const rgbai& foreground_color = {color::white},
        std::optional<rgbai> border_color_arg = std::nullopt,
        const rgbai& background_color = {color::black, 0}
    );
}
