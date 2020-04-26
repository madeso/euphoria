#ifndef EUPHORIA_CORE_HASHGEN_SPRATOR_H
#define EUPHORIA_CORE_HASHGEN_SPRATOR_H

#include <optional>
#include <vector>

#include "core/rgb.h"


namespace euphoria::core
{
    struct Image;

    void
    RenderSprator
    (
        Image* image,
        int code,
        const Rgbai& foreground_color = {Color::White},
        const std::optional<Rgbai> border_color_arg = std::nullopt,
        const Rgbai& background_color = {Color::Black, 0}
    );

    void
    RenderSprator
    (
        std::vector<Image>* images,
        int code,
        const Rgbai& foreground_color = {Color::White},
        const std::optional<Rgbai> border_color_arg = std::nullopt,
        const Rgbai& background_color = {Color::Black, 0}
    );
}


#endif  // EUPHORIA_CORE_HASHGEN_SPRATOR_H

