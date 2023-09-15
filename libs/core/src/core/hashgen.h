#pragma once

#include <optional>


#include "core/rgb.h"
#include "core/ints.h"

namespace eu::core
{
    struct Image;
}


namespace eu::core
{
    void render_retro(Image* image, U32 code);
}

// converted to c++ from https://github.com/donpark/identicon/blob/master/identicon-canvas/identicon_canvas.js
namespace eu::core
{
    void render_identicon(Image* image, U32 code);
}

// based on https://github.com/yurkth/sprator/
namespace eu::core
{
    void
    render_sprator
    (
        Image* image,
        U32 code,
        const Rgbai& foreground_color = {NamedColor::white},
        std::optional<Rgbai> border_color_arg = std::nullopt,
        const Rgbai& background_color = {NamedColor::black, 0}
    );

    void
    render_sprator
    (
        std::vector<Image>* images,
        U32 code,
        const Rgbai& foreground_color = {NamedColor::white},
        std::optional<Rgbai> border_color_arg = std::nullopt,
        const Rgbai& background_color = {NamedColor::black, 0}
    );
}
