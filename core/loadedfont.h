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
        unsigned int code_point = 0;
        int bearing_x = 0;
        int bearing_y = 0;
        int advance = 0;
        Image image;
        float size = 0.0f;
    };


    typedef std::map<std::pair<unsigned int, unsigned int>, float> KerningMap;


    // represent a loaded font (or a part), but it not yet been converted
    // into a renderable data and texture.
    struct LoadedFont
    {
        std::map<unsigned int, LoadedGlyph> codepoint_to_glyph;
        KerningMap kerning;
        std::map<std::string, unsigned int> private_use_aliases;
        unsigned int next_private_use = 0xE000;
        int line_height = -1;

        unsigned int
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
        unsigned int font_size,
        const std::string& chars
    );


    LoadedFont
    GetCharactersFromFont
    (
        std::shared_ptr<MemoryChunk> file_memory,
        unsigned int font_size,
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
}

#endif // EUPHORIA_CORE_FONTS_H
