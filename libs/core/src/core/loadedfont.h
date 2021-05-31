#pragma once


#include <string>
#include <map>

#include "core/image.h"


namespace euphoria::core::vfs
{
    struct file_system;
    struct file_path;
}


namespace euphoria::core
{
    // This represents a loaded glyph not yet placed on a texture image
    struct loaded_glyph
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
        core::image image;
        float size = 0.0f;
    };


    using kerning_map = std::map<std::pair<int, int>, float>;


    // represent a loaded font (or a part), but it not yet been converted
    // into a renderable data and texture.
    struct loaded_font
    {
        std::map<int, loaded_glyph> codepoint_to_glyph;
        kerning_map kerning;
        std::map<std::string, int> private_use_aliases;
        int next_private_use = 0xE000;
        int line_height = -1;

        int
        generate_new_index_from_private_use(const std::string& alias);

        void
        combine_with(const loaded_font& fc);
    };


    loaded_font
    load_characters_from_builtin8();


    loaded_font
    load_characters_from_builtin13();


    loaded_font
    get_characters_from_font
    (
        vfs::file_system* file_system,
        const vfs::file_path& font_file,
        int font_size,
        const std::string& chars
    );


    loaded_font
    get_characters_from_font
    (
        std::shared_ptr<memory_chunk> file_memory,
        int font_size,
        const std::string& chars
    );

    loaded_font
    get_characters_from_single_image
    (
        vfs::file_system* fs,
        const vfs::file_path& image_file,
        const std::string& image_alias,
        float image_scale,
        float image_bearing_x,
        float image_bearing_y,
        float image_advance
    );

    loaded_font
    get_characters_from_single_image
    (
        const image& image,
        const std::string& image_alias,
        float image_scale,
        float image_bearing_x,
        float image_bearing_y,
        float image_advance
    );
}

