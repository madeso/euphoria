#pragma once



#include <map>

#include "eu/core/image.h"


namespace eu
{
    struct MemoryChunk;
}

namespace eu::core
{
    // This represents a loaded glyph not yet placed on a texture image
    struct LoadedGlyph
    {
        //
        //              width_
        //           <--------->
        //           +---------+
        // bearingX  |         |   |              |
        // ------->  |         |   |  bearingY    |
        // origin    |         |   |              | height
        // X.........|.........|....... baseline  |
        //           |         |                  |
        //           |         |                  |
        //           +---------+
        //
        // ------------------------------> advance (x to next glyph)
        //

        bool valid = false;
        int code_point = 0;
        int bearing_x = 0;
        int bearing_y = 0;
        int advance = 0;
        Image image;
        float size = 0.0f;
    };


    using KerningMap = std::map<std::pair<int, int>, float>;


    // represent a loaded font (or a part), but it not yet been converted
    // into a renderable data and texture.
    struct LoadedFont
    {
        std::map<int, LoadedGlyph> codepoint_to_glyph;
        KerningMap kerning;
        std::map<std::string, int> private_use_aliases;
        int next_private_use = 0xE000;
        float ascent = 0.0f;
        float descent = 0.0f;
        float line_gap = 0.0f;

        int
        generate_new_index_from_private_use(const std::string& alias);
        
        bool
        load_characters_from_font
        (
            const MemoryChunk& file_memory,
            int font_size,
            const std::string& chars
        );
    };

    LoadedFont
    get_characters_from_single_image
    (
        const std::string& image_file,
        const std::string& image_alias,
        float image_scale,
        float image_bearing_x,
        float image_bearing_y,
        float image_advance
    );

    LoadedFont
    get_characters_from_single_image
    (
        const Image& image,
        const std::string& image_alias,
        float image_scale,
        float image_bearing_x,
        float image_bearing_y,
        float image_advance
    );
}

