#ifndef EUPHORIA_CORE_COLLAGE_H
#define EUPHORIA_CORE_COLLAGE_H

#include <vector>
#include <optional>

#include "core/vec2.h"
#include "core/size.h"


namespace euphoria::core
{
    struct image;
    struct rgbi;

    std::vector<Sizei> collect_sizes
    (
        const std::vector<image>& images,
        int padding
    );

    image
    draw_image
    (
        const std::vector<vec2i>& positions,
        const std::vector<image>& images,
        const Sizei& size,
        const rgbi& background_color
    );

    Sizei
    pack_tight
    (
        const Sizei& default_size,
        std::vector<vec2i>* positions,
        const std::vector<image>& images,
        int padding
    );


    ////////////////////////////////////////////////

    std::vector<vec2i>
    pack_image
    (
        const Sizei& image_size,
        const std::vector<image>& images,
        const std::vector<std::string>& files,
        int padding
    );


    std::optional<image>
    pack_image
    (
        const std::vector<image>& images,
        const std::vector<std::string>& files,
        const Sizei& requested_size,
        int padding,
        rgbi background_color,
        bool pack_image
    );


    ////////////////////////////////////////////////


    std::pair<std::vector<vec2i>, Sizei>
    grid_layout
    (
        const std::vector<image>& images,
        int padding,
        bool top_to_bottom
    );

    image
    grid_layout
    (
        const std::vector<image>& images,
        int padding,
        rgbi background_color,
        bool top_to_bottom
    );

}


#endif  // EUPHORIA_CORE_COLLAGE_H

