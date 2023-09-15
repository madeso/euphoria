#pragma once




#include "core/table.h"
#include "core/rgb.h"

#include "core/imagefilters.h"


namespace eu::core
{
    struct Image;


    struct ImageMapAction
    {
        char to;
        Rgbi from_color;
    };


    // todo(Gustav): add dithering support
    Table<char>
    from_image_to_string_table
    (
        const Image& img,
        const std::vector<ImageMapAction>& map
    );


    Table<char>
    from_image_to_string_table_exact
    (
        const Image& img,
        const std::vector<ImageMapAction>& map, char missing
    );


    Table<char>
    from_image_to_string_table(const Image& img, bool shorter, Grayscale grayscale = Grayscale::r);


    std::vector<std::string>
    to_strings(const Table<char>& t);


    // todo(Gustav): add function to print image using colored unicode blocks
}
