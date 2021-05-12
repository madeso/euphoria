#include "core/loadedfont.h"

#include <set>

#include "ft2build.h"
#include FT_FREETYPE_H

#include "font8x8/font8x8_basic.h"
#include "font8x13.h"

#include "core/log.h"
#include "core/assert.h"
#include "core/noncopyable.h"
#include "core/image_draw.h"
#include "core/utf8.h"
#include "core/vfs.h"
#include "core/cint.h"
#include "core/memorychunk.h"
#include "core/vfs_path.h"


namespace euphoria::core
{
    LOG_SPECIFY_DEFAULT_LOGGER("core.font")

    namespace
    {
        std::string_view error_to_string(FT_Error err)
        {
            #undef __FTERRORS_H__
            #define FT_ERRORDEF( e, v, s )  case e: return s;
            #define FT_ERROR_START_LIST     switch (err) {
            #define FT_ERROR_END_LIST       }
            #include FT_ERRORS_H
            return "[unknown error]";
        }


        [[nodiscard]]
        bool
        is_error(FT_Error err)
        {
            if(err == 0)
            {
                return false;
            }

            LOG_ERROR("FONT Error: {1} ({0})", err, error_to_string(err));
            return true;
        }


        void
        log_error(FT_Error err)
        {
            if(err == 0)
            {
                return;
            }
            LOG_ERROR("FONT Error: {1} ({0})", err, error_to_string(err));
        }
    }


    struct freetype_library
    {
        NONCOPYABLE_CONSTRUCTOR(freetype_library);
        NONCOPYABLE_ASSIGNMENT(freetype_library);

        freetype_library(freetype_library&& other) : library(other.library) {other.library = nullptr;};
        freetype_library&
        operator=(freetype_library&& other) {library = other.library; other.library = nullptr; return *this;};

        FT_Library library;


        explicit
        freetype_library(FT_Library lib)
            : library(lib)
        {
        }


        [[nodiscard]]
        static
        std::optional<freetype_library>
        create()
        {
            FT_Library library = nullptr;
            if(is_error(FT_Init_FreeType(&library)))
            {
                return std::nullopt;
            }
            else
            {
                return freetype_library{library};
            }
        }


        ~freetype_library()
        {
            if(library == nullptr )
            {
                return;
            }
            log_error(FT_Done_FreeType(library));
        }
    };


    struct freetype_face
    {
        FT_Face face;
        float size;

        NONCOPYABLE_CONSTRUCTOR(freetype_face);
        NONCOPYABLE_ASSIGNMENT(freetype_face);

        freetype_face(freetype_face&& other)
            : face(other.face)
            , size(other.size)
        {
            other.face = nullptr;
        }

        freetype_face&
        operator=(freetype_face&& other)
        {
            face = other.face;
            size = other.size;
            other.face = nullptr;
            return *this;
        }


        freetype_face(FT_Face f, int s)
            : face(f)
            , size(static_cast<float>(s))
        {
        }


        [[nodiscard]]
        static
        std::optional<freetype_face>
        create
        (
            freetype_library* lib,
            std::shared_ptr<memory_chunk> memory,
            int size
        )
        {
            FT_Face face = nullptr;

            int face_index = 0;
            if
            (
                is_error
                (
                    FT_New_Memory_Face
                    (
                        lib->library,
                        reinterpret_cast<FT_Byte*>(memory->get_data()), // NOLINT
                        memory->get_size(),
                        face_index,
                        &face
                    )
                )
            )
            {
                LOG_ERROR("Failed to load font");
                return std::nullopt;
            }

            if(is_error(FT_Set_Pixel_Sizes(face, 0, size)))
            {
                FT_Done_Face(face);
                return std::nullopt;
            }

            return freetype_face{face, size};
        }


        [[nodiscard]] loaded_glyph
        load_glyph(int code_point) const
        {
            const auto error = FT_Load_Char(face, code_point, FT_LOAD_RENDER);
            if(error != 0)
            {
                LOG_ERROR("Failed to get char {0}", code_point);
                return loaded_glyph();
            }

            FT_GlyphSlot slot = face->glyph;

            loaded_glyph ch;
            ch.code_point= code_point;
            ch.size = size;
            ch.bearing_x = slot->bitmap_left;
            ch.bearing_y = slot->bitmap_top;
            ch.valid = true;
            ch.advance = slot->advance.x >> 6;
            // pen_y += slot->advance.y >> 6;
            if(slot->bitmap.width == 0)
            {
                ch.image.make_invalid();
            }
            else
            {
                const bool upside_down = slot->bitmap.pitch > 0;

                ch.image.setup_with_alpha_support
                (
                    Cunsigned_int_to_int(slot->bitmap.width),
                    Cunsigned_int_to_int(slot->bitmap.rows)
                );
                auto* buffer = slot->bitmap.buffer;

                for(int y = 0; y < ch.image.height; y += 1)
                {
                    for(int x = 0; x < ch.image.width; x += 1)
                    {
                        const int target_y = upside_down
                            ? ch.image.height - (y + 1)
                            : y
                            ;

                        ch.image.set_pixel
                        (
                            x,
                            target_y,
                            255,
                            255,
                            255,
                            buffer[ch.image.width * y + x]
                        );
                    }
                }
            }

            return ch;
        }


        ~freetype_face()
        {
            if(face == nullptr) { return; }
            FT_Done_Face(face);
        }
    };


    int
    loaded_font::generate_new_index_from_private_use(const std::string& alias)
    {
        // detect existing private use alias!
        const auto pu = next_private_use;
        next_private_use += 1;
        private_use_aliases[alias] = pu;
        return pu;
    }


    void
    loaded_font::combine_with(const loaded_font& fc)
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
                LOG_ERROR("Multiple codepoints for {0} found when trying to combine", code_point);
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

        line_height = std::max(line_height, fc.line_height);
    }


    template<typename Glyphs>
    loaded_font
    get_character_from_builtin8
    (
        const int start_codepoint,
        int end_codepoint,
        Glyphs glyphs
    )
    {
        ASSERTX(start_codepoint < end_codepoint, start_codepoint, end_codepoint);
        const auto number_of_glyphs = (end_codepoint+1) - start_codepoint;
        loaded_font font;
        font.line_height = 8;

        for
        (
            int glyph_index=0;
            glyph_index < number_of_glyphs;
            glyph_index+=1
        )
        {
            const auto code_point = glyph_index + start_codepoint;
            loaded_glyph glyph;
            glyph.image.setup_with_alpha_support(8, 8, 0);

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
                        draw_square(&glyph.image, {color::white}, x, y, 1);
                    }
                }
            }

            glyph.size = static_cast<float>(glyph.image.height);
            glyph.bearing_y = glyph.image.height + 0;
            glyph.bearing_x = 0;
            glyph.advance = glyph.image.width + 0;
            glyph.code_point= code_point;
            glyph.valid = true;
            font.codepoint_to_glyph[code_point] = glyph;
        }

        return font;
    }


    loaded_font
    load_characters_from_builtin13()
    {
        loaded_font font;
        font.line_height = 13;

        for(int codepoint=32; codepoint < 127; codepoint+=1)
        {
            loaded_glyph glyph;
            glyph.image.setup_with_alpha_support(8, 13, 0);

            const auto glyph_index = codepoint - 32;

            for(int y = 0; y < 13; y += 1)
            {
                for(int x = 0; x < 8; x += 1)
                {
                    const bool pixel = 0 !=
                    (
                        FONT8X13_RASTERS[glyph_index * 13 + y] & (1 << (8-x))
                    );
                    if(pixel)
                    {
                        draw_square(&glyph.image, {color::white}, x, y, 1);
                    }
                }
            }

            glyph.size = static_cast<float>(glyph.image.height);
            glyph.bearing_y = glyph.image.height + 0;
            glyph.bearing_x = 0;
            glyph.advance = glyph.image.width + 0;
            glyph.code_point= codepoint;
            glyph.valid = true;
            font.codepoint_to_glyph[codepoint] = glyph;
        }

        return font;
    }


    loaded_font
    load_characters_from_builtin8()
    {
        loaded_font font;
        // todo(Gustav): Add more characters
        font.combine_with
        (
            get_character_from_builtin8(0x0000, 0x007F, font8x8_basic)
        );
        return font;
    }


    loaded_font
    get_characters_from_font
    (
        vfs::file_system* file_system,
        const vfs::file_path& font_file,
        int font_size,
        const std::string& chars
    )
    {
        auto file_memory = file_system->read_file(font_file);

        if(file_memory == nullptr)
        {
            LOG_ERROR("Unable to open {0}", font_file);
            return loaded_font{};
        }

        return get_characters_from_font
        (
            file_memory,
            font_size,
            chars
        );
    }


    loaded_font
    get_characters_from_font
    (
        std::shared_ptr<memory_chunk> file_memory,
        int font_size,
        const std::string& chars
    )
    {
        auto created_lib = freetype_library::create();
        if(created_lib.has_value() == false) { return loaded_font{}; }
        auto lib = std::move(*created_lib);

        auto loaded_face = freetype_face::create
        (
            &lib,
            file_memory,
            font_size
        );
        if(loaded_face.has_value() == false) { return loaded_font{}; }
        auto f = std::move(*loaded_face);

        std::vector<int> code_points;
        utf8_to_codepoints
        (
            chars,
            [&](int cp){code_points.emplace_back(cp);}
        );

        loaded_font fontchars {};
        for(char code_point: code_points)
        {
            loaded_glyph cc = f.load_glyph(code_point);
            if(!cc.valid)
            {
                LOG_INFO("Invalid codepoint {0}", code_point);
                continue;
            }
            fontchars.codepoint_to_glyph[code_point] = cc;
        }

        if(f.face == nullptr)
        {
            LOG_ERROR("Missing font face!");
            return fontchars;
        }
        if(f.face->size == nullptr) { return fontchars; }

        fontchars.line_height = f.face->size->metrics.height;

        const FT_Long use_kerning = FT_HAS_KERNING(f.face);

        LOG_INFO
        (
            "Loaded {0} characters",
            fontchars.codepoint_to_glyph.size()
        );
        LOG_INFO("kerning: {0}", static_cast<int>(use_kerning));

        const float scale = 1 / static_cast<float>(font_size);

        if(use_kerning != 1)
        {
            return fontchars;
        }

        for(const char previous: code_points)
        {
            for(const char current: code_points)
            {
                if(previous == current)
                {
                    continue;
                }
                const auto previous_index = FT_Get_Char_Index(f.face, previous);
                const auto current_index = FT_Get_Char_Index(f.face, current);
                FT_Vector delta {};
                FT_Get_Kerning
                (
                    f.face,
                    previous_index,
                    current_index,
                    FT_KERNING_DEFAULT,
                    &delta
                );
                const int dx = delta.x >> 6;
                if(dx != 0)
                {
                    fontchars.kerning.insert
                    (
                        kerning_map::value_type
                        (
                            kerning_map::key_type(previous, current),
                            static_cast<float>(dx) * scale
                        )
                    );
                }
            }
        }

        return fontchars;
    }


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
    )
    {
        const auto loaded = load_image
        (
            fs,
            image_file,
            alpha_load::Keep
        );

        if(loaded.error.empty() == false)
        {
            LOG_ERROR("Failed to load font image {}", image_file);
            return loaded_font{};
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


    loaded_font
    get_characters_from_single_image
    (
        const image& image,
        const std::string& image_alias,
        float image_scale,
        float image_bearing_x,
        float image_bearing_y,
        float image_advance
    )
    {
        loaded_font font;

        const auto s = 1 / image_scale;
        loaded_glyph glyph;
        glyph.size = s * static_cast<float>(image.height);
        glyph.bearing_y = Cfloat_to_int(s * static_cast<float>(image.height) + image_bearing_y);
        glyph.bearing_x = Cfloat_to_int(image_bearing_x);
        glyph.advance = Cfloat_to_int(s * static_cast<float>(image.width) + image_advance);
        glyph.code_point= font.generate_new_index_from_private_use(image_alias);
        // todo(Gustav): add ability to clip image
        glyph.image = image;
        font.codepoint_to_glyph[glyph.code_point] = glyph;

        return font;
    }

}

