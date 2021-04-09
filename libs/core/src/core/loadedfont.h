#ifndef EUPHORIA_CORE_FONTS_H
#define EUPHORIA_CORE_FONTS_H

#include <string>
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
        int line_height = -1;

        int
        NewPrivateUse(const std::string& alias);

        void
        CombineWith(const LoadedFont& fc);
    };


    LoadedFont
    LoadCharactersFromBuiltin8();


    LoadedFont
    LoadCharactersFromBuiltin13();


    LoadedFont
    GetCharactersFromFont
    (
        vfs::FileSystem* file_system,
        const vfs::FilePath& font_file,
        int font_size,
        const std::string& chars
    );


    LoadedFont
    GetCharactersFromFont
    (
        std::shared_ptr<MemoryChunk> file_memory,
        int font_size,
        const std::string& chars
    );

    LoadedFont
    GetCharactersFromSingleImage
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
    GetCharactersFromSingleImage
    (
        const Image& image,
        const std::string& image_alias,
        float image_scale,
        float image_bearing_x,
        float image_bearing_y,
        float image_advance
    );
}

#endif // EUPHORIA_CORE_FONTS_H
