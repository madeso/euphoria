#pragma once

#include "io/json.h"

namespace eu::files::font
{
    struct FontFile
    {
        std::string file;
        std::string characters;
    };

    struct SingleImage
    {
        std::string file;
        std::string alias;
        float scale = 1.0f;
        int bearing_x;
        int bearing_y;
        int advance;
    };

    struct Root
    {
        int size;

        std::vector<FontFile> fonts;
        std::vector<SingleImage> images;
        bool builtin8 = false;
        bool builtin13 = false;
    };

    JSON_PARSE_FUNC(Root);
}
