#pragma once


#include <optional>

#include "core/vec2.h"
#include "core/size2.h"


namespace euphoria::core
{
    struct Image;
    struct Rgbi;

    std::vector<Size2i> collect_sizes
    (
        const std::vector<Image>& images,
        int padding
    );

    Image
    draw_image
    (
        const std::vector<Vec2i>& positions,
        const std::vector<Image>& images,
        const Size2i& size,
        const Rgbi& background_color
    );

    Size2i
    pack_tight
    (
        const Size2i& default_size,
        std::vector<Vec2i>* positions,
        const std::vector<Image>& images,
        int padding
    );


    ////////////////////////////////////////////////

    std::vector<Vec2i>
    pack_image
    (
        const Size2i& image_size,
        const std::vector<Image>& images,
        const std::vector<std::string>& files,
        int padding
    );


    std::optional<Image>
    pack_image
    (
        const std::vector<Image>& images,
        const std::vector<std::string>& files,
        const Size2i& requested_size,
        int padding,
        Rgbi background_color,
        bool pack_image
    );


    ////////////////////////////////////////////////


    std::pair<std::vector<Vec2i>, Size2i>
    grid_layout
    (
        const std::vector<Image>& images,
        int padding,
        bool top_to_bottom
    );

    Image
    grid_layout
    (
        const std::vector<Image>& images,
        int padding,
        Rgbi background_color,
        bool top_to_bottom
    );

}
