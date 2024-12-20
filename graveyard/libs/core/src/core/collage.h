#pragma once


#include <optional>

#include "base/vec2.h"
#include "base/size2.h"


namespace eu
{
    struct Rgbi;
}

namespace eu::core
{
    struct Image;

    std::vector<size2i> collect_sizes
    (
        const std::vector<Image>& images,
        int padding
    );

    Image
    draw_image
    (
        const std::vector<vec2i>& positions,
        const std::vector<Image>& images,
        const size2i& size,
        const Rgbi& background_color
    );

    size2i
    pack_tight
    (
        const size2i& default_size,
        std::vector<vec2i>* positions,
        const std::vector<Image>& images,
        int padding
    );


    ////////////////////////////////////////////////

    std::vector<vec2i>
    pack_image
    (
        const size2i& image_size,
        const std::vector<Image>& images,
        const std::vector<std::string>& files,
        int padding
    );


    std::optional<Image>
    pack_image
    (
        const std::vector<Image>& images,
        const std::vector<std::string>& files,
        const size2i& requested_size,
        int padding,
        Rgbi background_color,
        bool pack_image
    );


    ////////////////////////////////////////////////


    std::pair<std::vector<vec2i>, size2i>
    lay_out_in_a_grid
    (
        const std::vector<Image>& images,
        int padding,
        bool top_to_bottom
    );


    // todo(Gustav): rename to lay_out_on_a_image
    Image
    lay_out_in_a_grid
    (
        const std::vector<Image>& images,
        int padding,
        Rgbi background_color,
        bool top_to_bottom
    );

}
