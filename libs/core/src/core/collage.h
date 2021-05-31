#pragma once

#include <vector>
#include <optional>

#include "core/vec2.h"
#include "core/size2.h"


namespace euphoria::core
{
    struct image;
    struct rgbi;

    std::vector<size2i> collect_sizes
    (
        const std::vector<image>& images,
        int padding
    );

    image
    draw_image
    (
        const std::vector<vec2i>& positions,
        const std::vector<image>& images,
        const size2i& size,
        const rgbi& background_color
    );

    size2i
    pack_tight
    (
        const size2i& default_size,
        std::vector<vec2i>* positions,
        const std::vector<image>& images,
        int padding
    );


    ////////////////////////////////////////////////

    std::vector<vec2i>
    pack_image
    (
        const size2i& image_size,
        const std::vector<image>& images,
        const std::vector<std::string>& files,
        int padding
    );


    std::optional<image>
    pack_image
    (
        const std::vector<image>& images,
        const std::vector<std::string>& files,
        const size2i& requested_size,
        int padding,
        rgbi background_color,
        bool pack_image
    );


    ////////////////////////////////////////////////


    std::pair<std::vector<vec2i>, size2i>
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
