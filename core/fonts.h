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

        bool        valid {false};
        unsigned int code_point {0};
        int         bearing_x {0};
        int         bearing_y {0};
        int         advance {0};
        core::Image image;
        float       size;
    };

    typedef std::map<std::pair<unsigned int, unsigned int>, float> KerningMap;

    // represent a loaded font (or a part), but it not yet been converted
    // into a renderable data and texture.
    struct LoadedFont
    {
        std::map<unsigned int, LoadedGlyph> codepoint_to_glyph;
        KerningMap               kerning;

        void
        CombineWith(const LoadedFont& fc);
    };

    void
    LoadCharactersFromBuiltin(core::LoadedFont* font, int scale);

    LoadedFont
    GetCharactersFromFont(
            const std::string& font_file,
            unsigned int       font_size,
            const std::string& chars);
}

#endif // EUPHORIA_CORE_FONTS_H
