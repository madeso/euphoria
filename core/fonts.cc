#include "core/fonts.h"

#include "core/log.h"
#include "core/assert.h"
#include "core/noncopyable.h"
#include "core/image_draw.h"

#include <ft2build.h>
#include FT_FREETYPE_H

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


    // todo(Gustav): add better utf8 support
    std::string
    ConvertCharToIndex(char c)
    {
        return std::string {c};
    }

    namespace
    {
        // todo: use instad FT_Get_Char_Index( FT_Face face, FT_ULong charcode);
        FT_UInt
        CharToFt(char c)
        {
            // todo: support unicode/utf-8 character encoding
            return static_cast<FT_UInt>(c);
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
        LoadGlyph(char c)
        {
            const FT_Error error
                    = FT_Load_Char(face, CharToFt(c), FT_LOAD_RENDER);
            if(error != 0)
            {
                LOG_ERROR("Failed to get char");
                return LoadedGlyph();
            }

            FT_GlyphSlot slot = face->glyph;

            LoadedGlyph ch;
            ch.c         = c;
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

    

    void
    LoadedFont::CombineWith(const LoadedFont& fc)
    {
        for(const auto& c: fc.chars)
        {
            chars.push_back(c);
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

    core::LoadedFont
    GetCharacterFromBuiltin(const std::string& text, int s)
    {
        core::LoadedFont font;

        const int size = 8 * s;

        core::LoadedGlyph glyph;
        glyph.image.SetupWithAlphaSupport(size, size, 0);

        core::DrawText(
                &glyph.image, core::vec2i::Zero(), text, core::Rgbi {255}, s);

        glyph.size      = glyph.image.GetHeight();
        glyph.bearing_y = glyph.image.GetHeight() + 0;
        glyph.bearing_x = 0;
        glyph.advance   = glyph.image.GetWidth() + 0;
        glyph.c         = text;
        font.chars.emplace_back(glyph);

        return font;
    }

    void
    LoadCharactersFromBuiltin(core::LoadedFont* font, int scale)
    {
        for(int c = ' '; c < 128; c += 1)
        {
            const std::string text(1, static_cast<char>(c));
            font->CombineWith(GetCharacterFromBuiltin(text, scale));
        }
    }

    LoadedFont
    GetCharactersFromFont(
            const std::string& font_file,
            unsigned int       font_size,
            const std::string& chars)
    {
        FreetypeLibrary lib;
        FreetypeFace    f(&lib, font_file, font_size);

        LoadedFont fontchars {};
        fontchars.chars.reserve(chars.length());
        for(char c: chars)
        {
            LoadedGlyph cc = f.LoadGlyph(c);
            if(!cc.valid)
            {
                continue;
            }
            fontchars.chars.push_back(cc);
        }

        const FT_Long use_kerning = FT_HAS_KERNING(f.face);

        LOG_INFO(
                "Loaded " << fontchars.chars.size() << " characters from "
                          << font_file);
        LOG_INFO("kerning: " << static_cast<int>(use_kerning));

        const float scale = 1 / static_cast<float>(font_size);

        if(use_kerning == 1)
        {
            for(const char previous: chars)
            {
                for(const char current: chars)
                {
                    if(previous == current)
                    {
                        continue;
                    }
                    const std::string previous_c = ConvertCharToIndex(previous);
                    const std::string current_c  = ConvertCharToIndex(current);
                    FT_Vector         delta {};
                    FT_Get_Kerning(
                            f.face,
                            CharToFt(previous),
                            CharToFt(current),
                            FT_KERNING_DEFAULT,
                            &delta);
                    int dx = delta.x >> 6;
                    if(dx != 0)
                    {
                        fontchars.kerning.insert(KerningMap::value_type(
                                KerningMap::key_type(previous_c, current_c),
                                dx * scale));
                    }
                }
            }
        }

        return fontchars;
    }
}

