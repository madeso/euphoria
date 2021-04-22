#pragma once

#include <vector>
#include <string>

#include "core/table.h"
#include "core/rgb.h"

#include "core/imagefilters.h"


namespace euphoria::core
{
    struct image;

    struct image_map_action
    {
        char to;
        rgbi from_color;
    };

    // todo(Gustav): add dithering support
    Table<char>
    image_to_string_table(
            const image&                       img,
            const std::vector<image_map_action>& map);

    Table<char>
    image_to_string_table_exact(
            const image&                       img,
            const std::vector<image_map_action>& map, char missing);

    Table<char>
    image_to_string_table(const image& img, bool shorter, grayscale grayscale = grayscale::r);

    std::vector<std::string>
    to_strings(const Table<char>& t);

    // todo(Gustav): add function to print image using colored unicode blocks
}
