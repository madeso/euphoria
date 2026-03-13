#include "core/loadedfont.h"

#include <set>
#include <optional>
#include <map>

#include "eustb_truetype.h"

#include "log/log.h"

#include "assert/assert.h"

#include "base/cint.h"
#include "base/memorychunk.h"

#include "core/utf8.h"



namespace eu::core
{
    struct FontData
    {
        stbtt_fontinfo font;
        float size;
        bool was_loaded;
        int line_height = 0;
        std::vector<stbtt_kerningentry> kernings;
        std::map<int, int> index_to_unicode;

        FontData(const unsigned char* ttf_buffer, float s)
            : font{}
            , size{s}
            , was_loaded{stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0)) == 1}
        {
            if(was_loaded == false) {return;}

            const int count = stbtt_GetKerningTableLength(&font);
            kernings.reserve(count);
            stbtt_GetKerningTable(&font, kernings.data(), count);
            
            int ascent = 0;
            int descent = 0;
            int line_gap = 0;
            stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);
            line_height = ascent - descent + line_gap;
        }

        void define_codepoint(int cp)
        {
            const int index = stbtt_FindGlyphIndex(&font, cp);
            index_to_unicode[index] = cp;
        }

        [[nodiscard]] std::optional<int> from_glyph_index_to_unicode(int glyph) const
        {
            auto found = index_to_unicode.find(glyph);
            if(found == index_to_unicode.end())
            {
                return {};
            }
            else
            {
                return found->second;
            }
        }

        [[nodiscard]] LoadedGlyph
        load_glyph(int code_point) const
        {
            int width=0; int height=0;
            int xoffset=0; int yoffset=0;
            const auto scale = stbtt_ScaleForPixelHeight(&font, size);
            unsigned char *bitmap = stbtt_GetCodepointBitmap
            (
                &font,
                scale,
                scale,
                code_point,
                &width,
                &height,
                &xoffset,
                &yoffset
            );

            LoadedGlyph ch;
            ch.code_point= code_point;
            ch.size = size;
            ch.bearing_x = xoffset;
            ch.bearing_y = -yoffset;
            ch.valid = true;
            {
                int advance_width = 0;
                stbtt_GetCodepointHMetrics(&font, code_point, &advance_width, nullptr);
                ch.advance = int_from_float(float_from_int(advance_width) * scale);
            }
            if(width == 0 && height == 0)
            {
                ch.image.make_invalid();
            }
            else
            {
                ch.image.setup_with_alpha_support(width, height);

                for(int y = 0; y < ch.image.height; y += 1)
                {
                    for(int x = 0; x < ch.image.width; x += 1)
                    {
                        const auto alpha = bitmap[ch.image.width * y + x];
                        ch.image.set_pixel
                        (
                            x, ch.image.height-y-1,
                            255, 255, 255,
                            alpha
                        );
                    }
                }
            }

            if(bitmap != nullptr)
            {
                stbtt_FreeBitmap(bitmap, nullptr);
            }

            return ch;
        }
    };


    int
    LoadedFont::generate_new_index_from_private_use(const std::string& alias)
    {
        // detect existing private use alias!
        const auto pu = next_private_use;
        next_private_use += 1;
        private_use_aliases[alias] = pu;
        return pu;
    }


    void
    LoadedFont::combine_with(const LoadedFont& fc)
    {
        std::map<int, int> pus;
        for(const auto& [alias, id]: fc.private_use_aliases)
        {
            pus[id] = generate_new_index_from_private_use(alias);
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
                LOG_ERR("Multiple codepoints for {0} found when trying to combine", code_point);
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
                LOG_ERR("Multiple kernings found when trying to combine");
            }
        }

        line_height = std::max(line_height, fc.line_height);
    }


    LoadedFont
    get_characters_from_font
    (
        const MemoryChunk& file_memory,
        int font_size,
        const std::string& chars
    )
    {
        auto f = FontData
        {
            reinterpret_cast<const unsigned char*>(file_memory.bytes),
            float_from_int(font_size)
        };

        if(f.was_loaded == false) { return LoadedFont{}; }

        std::vector<int> code_points;
        calc_utf8_to_codepoints
        (
            chars,
            [&](int cp){code_points.emplace_back(cp);}
        );

        LoadedFont fontchars {};
        for(int code_point: code_points)
        {
            LoadedGlyph cc = f.load_glyph(code_point);
            if(!cc.valid)
            {
                LOG_INFO("Invalid codepoint {0}", code_point);
                continue;
            }
            fontchars.codepoint_to_glyph[code_point] = cc;
        }

        fontchars.line_height = f.line_height;

        const float scale = 1 / static_cast<float>(font_size);

        for(int cp: code_points)
        {
            f.define_codepoint(cp);
        }

        for(const auto& info: f.kernings)
        {
            const auto previous = f.from_glyph_index_to_unicode(info.glyph1);
            const auto current = f.from_glyph_index_to_unicode(info.glyph2);
            const int dx = info.advance;
            if(previous && current)
            {
                fontchars.kerning.insert
                (
                    KerningMap::value_type
                    (
                        KerningMap::key_type(*previous, *current),
                        static_cast<float>(dx) * scale
                    )
                );
            }
        }

        return fontchars;
    }

#if 0
    LoadedFont
    get_characters_from_single_image
    (
        io::FileSystem* fs,
        const io::FilePath& image_file,
        const std::string& image_alias,
        float image_scale,
        float image_bearing_x,
        float image_bearing_y,
        float image_advance
    )
    {
        const auto loaded = load_image
        (
            fs,
            image_file,
            AlphaLoad::keep
        );

        if(loaded.error.empty() == false)
        {
            LOG_ERR("Failed to load font image {}", image_file);
            return LoadedFont{};
        }

        return get_characters_from_single_image
        (
            loaded.image,
            image_alias,
            image_scale,
            image_bearing_x,
            image_bearing_y,
            image_advance
        );
    }
#endif


    LoadedFont
    get_characters_from_single_image
    (
        const Image& image,
        const std::string& image_alias,
        float image_scale,
        float image_bearing_x,
        float image_bearing_y,
        float image_advance
    )
    {
        LoadedFont font;

        const auto s = 1 / image_scale;
        LoadedGlyph glyph;
        glyph.size = s * static_cast<float>(image.height);
        glyph.bearing_y = int_from_float(s * static_cast<float>(image.height) + image_bearing_y);
        glyph.bearing_x = int_from_float(image_bearing_x);
        glyph.advance = int_from_float(s * static_cast<float>(image.width) + image_advance);
        glyph.code_point= font.generate_new_index_from_private_use(image_alias);
        // todo(Gustav): add ability to clip image
        glyph.image = image;
        font.codepoint_to_glyph[glyph.code_point] = glyph;

        return font;
    }

}

