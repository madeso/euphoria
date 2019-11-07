#include "core/fonts.h"

#include <set>

#include "core/log.h"
#include "core/assert.h"
#include "core/noncopyable.h"
#include "core/image_draw.h"
#include "core/utf8.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <font8x8/font8x8_basic.h>

namespace euphoria::core
{
    LOG_SPECIFY_DEFAULT_LOGGER("core.font")

    namespace
    {
        void
        Error(FT_Error err)
        {
            if(err == 0)
            {
                return;
            }
            LOG_ERROR("FONT Error: " << err);
        }

        void
        ErrorNoThrow(FT_Error err)
        {
            if(err == 0)
            {
                return;
            }
            LOG_ERROR("FONT Error: " << err);
        }
    }  // namespace


    struct FreetypeLibrary : Noncopyable
    {
        NONCOPYABLE_CONSTRUCTOR(FreetypeLibrary);
        NONCOPYABLE_ASSIGNMENT(FreetypeLibrary);
        NONCOPYABLE_MOVE_CONSTRUCTOR(FreetypeLibrary);
        NONCOPYABLE_MOVE_ASSIGNMENT(FreetypeLibrary);

        FT_Library library {nullptr};

        FreetypeLibrary()
        {
            Error(FT_Init_FreeType(&library));
        }

        ~FreetypeLibrary()
        {
            ErrorNoThrow(FT_Done_FreeType(library));
        }
    };

    struct FreetypeFace
    {
        FT_Face face;
        float   size;

        NONCOPYABLE_CONSTRUCTOR(FreetypeFace);
        NONCOPYABLE_ASSIGNMENT(FreetypeFace);
        NONCOPYABLE_MOVE_CONSTRUCTOR(FreetypeFace);
        NONCOPYABLE_MOVE_ASSIGNMENT(FreetypeFace);

        FreetypeFace(
                FreetypeLibrary*   lib,
                const std::string& path,
                unsigned int       size)
            : face(nullptr), size(size)
        {
            int face_index = 0;
            Error(FT_New_Face(lib->library, path.c_str(), face_index, &face));
            Error(FT_Set_Pixel_Sizes(face, 0, size));
        }

        LoadedGlyph
        LoadGlyph(unsigned int code_point)
        {
            const FT_Error error
                    = FT_Load_Char(face, code_point, FT_LOAD_RENDER);
            if(error != 0)
            {
                LOG_ERROR("Failed to get char");
                return LoadedGlyph();
            }

            FT_GlyphSlot slot = face->glyph;

            LoadedGlyph ch;
            ch.code_point= code_point;
            ch.size      = size;
            ch.bearing_x = slot->bitmap_left;
            ch.bearing_y = slot->bitmap_top;
            ch.valid     = true;
            ch.advance   = slot->advance.x >> 6;
            // pen_y += slot->advance.y >> 6;
            if(slot->bitmap.width == 0)
            {
                ch.image.MakeInvalid();
            }
            else
            {
                const bool upside_down = slot->bitmap.pitch > 0;

                ch.image.SetupWithAlphaSupport(
                        slot->bitmap.width, slot->bitmap.rows);
                auto* buffer = slot->bitmap.buffer;
                for(int y = 0; y < ch.image.GetHeight(); y += 1)
                {
                    for(int x = 0; x < ch.image.GetWidth(); x += 1)
                    {
                        const int target_y
                                = upside_down ? ch.image.GetHeight() - (y + 1)
                                              : y;

                        ch.image.SetPixel(
                                x,
                                target_y,
                                255,
                                255,
                                255,
                                buffer[ch.image.GetWidth() * y + x]);
                    }
                }

                // dump image to disk...
            }

            return ch;
        }

        ~FreetypeFace()
        {
            FT_Done_Face(face);
        }
    };

    unsigned int
    LoadedFont::NewPrivateUse(const std::string& alias)
    {
        // detect existing private use alias!
        const auto pu = next_private_use;
        next_private_use += 1;
        private_use_aliases[alias] = pu;
        return pu;
    }

    void
    LoadedFont::CombineWith(const LoadedFont& fc)
    {
        std::map<unsigned int, unsigned int> pus;
        for(const auto [alias, id]: fc.private_use_aliases)
        {
            pus[id] = NewPrivateUse(alias);
        }

        for(const auto& glyph_iterator: fc.codepoint_to_glyph)
        {
            auto code_point = glyph_iterator.first;
            auto found_pus = pus.find(code_point);
            if(found_pus != pus.end())
            {
                // private use: move to new private use to avoid collisions
                code_point = found_pus->second;
            }
            const auto found = codepoint_to_glyph.find(code_point);
            if(found == codepoint_to_glyph.end())
            {
                codepoint_to_glyph[code_point] = glyph_iterator.second;
            }
            else
            {
                LOG_ERROR("Multiple codepoints for " << code_point << " found when trying to combine");
            }
        }

        for(const auto& e: fc.kerning)
        {
            const auto found = kerning.find(e.first);
            if(found == kerning.end())
            {
                kerning.insert(e);
            }
            else
            {
                LOG_ERROR("Multiple kernings found when trying to combine");
            }
        }
    }

    template<typename Glyphs>
    core::LoadedFont
    GetCharacterFromBuiltin(const unsigned int start_codepoint, unsigned int end_codepoint, Glyphs glyphs)
    {
        ASSERTX(start_codepoint < end_codepoint, start_codepoint, end_codepoint);
        const auto number_of_glyphs = (end_codepoint+1) - start_codepoint;
        core::LoadedFont font;

        for(unsigned int glyph_index=0; glyph_index < number_of_glyphs; glyph_index+=1)
        {
            const auto code_point = glyph_index + start_codepoint;
            core::LoadedGlyph glyph;
            glyph.image.SetupWithAlphaSupport(8, 8, 0);

            for(int y = 0; y < 8; y += 1)
            {
                for(int x = 0; x < 8; x += 1)
                {
                    // extract pixel from character
                    // glyph is defined in "y down" order, fix by inverting sample point on y
                    bool pixel = 0 != (glyphs[glyph_index][7 - y] & 1 << x);
                    if(pixel)
                    {
                        // glyph.image.SetPixel(x, y, Color::White);
                        DrawSquare(&glyph.image, Color::White, x, y, 1);
                    }
                }
            }

            glyph.size      = glyph.image.GetHeight();
            glyph.bearing_y = glyph.image.GetHeight() + 0;
            glyph.bearing_x = 0;
            glyph.advance   = glyph.image.GetWidth() + 0;
            glyph.code_point= code_point;
            font.codepoint_to_glyph[code_point] = glyph;
        }

        return font;
    }

    core::LoadedFont
    LoadCharactersFromBuiltin()
    {
        core::LoadedFont font;
        // todo(Gustav): Add more characters
        font.CombineWith(GetCharacterFromBuiltin(0x0000, 0x007F, font8x8_basic));
        return font;
    }

    LoadedFont
    GetCharactersFromFont(
            const std::string& font_file,
            unsigned int       font_size,
            const std::string& chars)
    {
        FreetypeLibrary lib;
        FreetypeFace    f(&lib, font_file, font_size);

        std::vector<unsigned int> code_points;
        Utf8ToCodepoints(chars, [&](unsigned int cp){code_points.emplace_back(cp);});

        LoadedFont fontchars {};
        for(char code_point: code_points)
        {
            LoadedGlyph cc = f.LoadGlyph(code_point);
            if(!cc.valid)
            {
                continue;
            }
            fontchars.codepoint_to_glyph[code_point] = cc;
        }

        const FT_Long use_kerning = FT_HAS_KERNING(f.face);

        LOG_INFO(
                "Loaded " << fontchars.codepoint_to_glyph.size() << " characters from "
                          << font_file);
        LOG_INFO("kerning: " << static_cast<int>(use_kerning));

        const float scale = 1 / static_cast<float>(font_size);

        if(use_kerning == 1)
        {
            for(const char previous: code_points)
            {
                for(const char current: code_points)
                {
                    if(previous == current)
                    {
                        continue;
                    }
                    const auto previous_index = FT_Get_Char_Index(f.face, previous);
                    const auto current_index  = FT_Get_Char_Index(f.face, current);
                    FT_Vector         delta {};
                    FT_Get_Kerning(
                            f.face,
                            previous_index,
                            current_index,
                            FT_KERNING_DEFAULT,
                            &delta);
                    int dx = delta.x >> 6;
                    if(dx != 0)
                    {
                        fontchars.kerning.insert(KerningMap::value_type(
                                KerningMap::key_type(previous, current),
                                dx * scale));
                    }
                }
            }
        }

        return fontchars;
    }
}

