#pragma once



#include <map>

#include "core/image.h"


namespace euphoria::core::vfs
{
    struct FileSystem;
    struct FilePath;
}


namespace euphoria::core
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
        core::Image image;
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
        int line_height = -1;

        int
        generate_new_index_from_private_use(const std::string& alias);

        void
        combine_with(const LoadedFont& fc);
    };


    LoadedFont
    load_characters_from_builtin8();


    LoadedFont
    load_characters_from_builtin13();


    LoadedFont
    get_characters_from_font
    (
        vfs::FileSystem* file_system,
        const vfs::FilePath& font_file,
        int font_size,
        const std::string& chars
    );


    LoadedFont
    get_characters_from_font
    (
        std::shared_ptr<MemoryChunk> file_memory,
        int font_size,
        const std::string& chars
    );

    LoadedFont
    get_characters_from_single_image
    (
        vfs::FileSystem* fs,
        const vfs::FilePath& image_file,
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

