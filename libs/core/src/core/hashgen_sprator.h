#pragma once

#include <optional>
#include <vector>

#include "core/rgb.h"

// based on https://github.com/yurkth/sprator/

namespace euphoria::core
{
    struct image;

    void
    render_sprator
    (
        image* image,
        int code,
        const rgbai& foreground_color = {color::white},
        std::optional<rgbai> border_color_arg = std::nullopt,
        const rgbai& background_color = {color::black, 0}
    );

    void
    render_sprator
    (
        std::vector<image>* images,
        int code,
        const rgbai& foreground_color = {color::white},
        std::optional<rgbai> border_color_arg = std::nullopt,
        const rgbai& background_color = {color::black, 0}
    );
}
