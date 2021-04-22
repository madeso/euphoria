#ifndef EUPHORIA_CORE_HASHGEN_SPRATOR_H
#define EUPHORIA_CORE_HASHGEN_SPRATOR_H

#include <optional>
#include <vector>

#include "core/rgb.h"

// based on https://github.com/yurkth/sprator/

namespace euphoria::core
{
    struct image;

    void
    RenderSprator
    (
        image* image,
        int code,
        const rgbai& foreground_color = {color::white},
        std::optional<rgbai> border_color_arg = std::nullopt,
        const rgbai& background_color = {color::black, 0}
    );

    void
    RenderSprator
    (
        std::vector<image>* images,
        int code,
        const rgbai& foreground_color = {color::white},
        std::optional<rgbai> border_color_arg = std::nullopt,
        const rgbai& background_color = {color::black, 0}
    );
}


#endif  // EUPHORIA_CORE_HASHGEN_SPRATOR_H

