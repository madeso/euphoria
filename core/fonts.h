#ifndef EUPHORIA_CORE_FONTS_H
#define EUPHORIA_CORE_FONTS_H

#include <string>
#include <map>

#include "core/image.h"


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
        core::Image image;
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

    core::LoadedFont
    LoadCharactersFromBuiltin8();

    core::LoadedFont
    LoadCharactersFromBuiltin13();

    LoadedFont
    GetCharactersFromFont
    (
        const std::string& font_file,
        unsigned int font_size,
        const std::string& chars
    );
}

#endif // EUPHORIA_CORE_FONTS_H
