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
#include "core/memorychunk.h"
#include "core/vfs_path.h"


namespace euphoria::core
{
    LOG_SPECIFY_DEFAULT_LOGGER("core.font")

    namespace
    {
        std::string_view ErrorToString(FT_Error err)
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
        Error(FT_Error err)
        {
            if(err == 0)
            {
                return false;
            }

            LOG_ERROR("FONT Error: {1} ({0})", err, ErrorToString(err));
            return true;
        }


        void
        ErrorNoThrow(FT_Error err)
        {
            if(err == 0)
            {
                return;
            }
            LOG_ERROR("FONT Error: {1} ({0})", err, ErrorToString(err));
        }
    }


    struct FreetypeLibrary
    {
        NONCOPYABLE_CONSTRUCTOR(FreetypeLibrary);
        NONCOPYABLE_ASSIGNMENT(FreetypeLibrary);

        FreetypeLibrary(FreetypeLibrary&& other) : library(other.library) {other.library = nullptr;};
        FreetypeLibrary&
        operator=(FreetypeLibrary&& other) {library = other.library; other.library = nullptr; return *this;};

        FT_Library library;


        explicit
        FreetypeLibrary(FT_Library lib)
            : library(lib)
        {
        }


        [[nodiscard]]
        static
        std::optional<FreetypeLibrary>
        Create()
        {
            FT_Library library = nullptr;
            if(Error(FT_Init_FreeType(&library)))
            {
                return std::nullopt;
            }
            else
            {
                return FreetypeLibrary{library};
            }
        }


        ~FreetypeLibrary()
        {
            if(library == nullptr )
            {
                return;
            }
            ErrorNoThrow(FT_Done_FreeType(library));
        }
    };


    struct FreetypeFace
    {
        FT_Face face;
        float size;

        NONCOPYABLE_CONSTRUCTOR(FreetypeFace);
        NONCOPYABLE_ASSIGNMENT(FreetypeFace);

        FreetypeFace(FreetypeFace&& other)
            : face(other.face)
            , size(other.size)
        {
            other.face = nullptr;
        }

        FreetypeFace&
        operator=(FreetypeFace&& other)
        {
            face = other.face;
            size = other.size;
            other.face = nullptr;
            return *this;
        }


        FreetypeFace(FT_Face f, unsigned int s)
            : face(f)
            , size(s)
        {
        }


        [[nodiscard]]
        static
        std::optional<FreetypeFace>
        Create
        (
            FreetypeLibrary* lib,
            std::shared_ptr<MemoryChunk> memory,
            unsigned int size
        )
        {
            FT_Face face = nullptr;

            int face_index = 0;
            if
            (
                Error
                (
                    FT_New_Memory_Face
                    (
                        lib->library,
                        reinterpret_cast<FT_Byte*>(memory->GetData()),
                        memory->GetSize(),
                        face_index,
                        &face
                    )
                )
            )
            {
                LOG_ERROR("Failed to load font");
                return std::nullopt;
            }

            if(Error(FT_Set_Pixel_Sizes(face, 0, size)))
            {
                FT_Done_Face(face);
                return std::nullopt;
            }

            return FreetypeFace{face, size};
        }


        LoadedGlyph
        LoadGlyph(unsigned int code_point)
        {
            const auto error = FT_Load_Char(face, code_point, FT_LOAD_RENDER);
            if(error != 0)
            {
                LOG_ERROR("Failed to get char {0}", code_point);
                return LoadedGlyph();
            }

            FT_GlyphSlot slot = face->glyph;

            LoadedGlyph ch;
            ch.code_point= code_point;
            ch.size = size;
            ch.bearing_x = slot->bitmap_left;
            ch.bearing_y = slot->bitmap_top;
            ch.valid = true;
            ch.advance = slot->advance.x >> 6;
            // pen_y += slot->advance.y >> 6;
            if(slot->bitmap.width == 0)
            {
                ch.image.MakeInvalid();
            }
            else
            {
                const bool upside_down = slot->bitmap.pitch > 0;

                ch.image.SetupWithAlphaSupport
                (
                    slot->bitmap.width,
                    slot->bitmap.rows
                );
                auto* buffer = slot->bitmap.buffer;

                for(int y = 0; y < ch.image.GetHeight(); y += 1)
                {
                    for(int x = 0; x < ch.image.GetWidth(); x += 1)
                    {
                        const int target_y = upside_down
                            ? ch.image.GetHeight() - (y + 1)
                            : y
                            ;

                        ch.image.SetPixel
                        (
                            x,
                            target_y,
                            255,
                            255,
                            255,
                            buffer[ch.image.GetWidth() * y + x]
                        );
                    }
                }
            }

            return ch;
        }


        ~FreetypeFace()
        {
            if(face == nullptr) { return; }
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
    LoadedFont
    GetCharacterFromBuiltin8
    (
        const unsigned int start_codepoint,
        unsigned int end_codepoint,
        Glyphs glyphs
    )
    {
        ASSERTX(start_codepoint < end_codepoint, start_codepoint, end_codepoint);
        const auto number_of_glyphs = (end_codepoint+1) - start_codepoint;
        LoadedFont font;
        font.line_height = 8;

        for
        (
            unsigned int glyph_index=0;
            glyph_index < number_of_glyphs;
            glyph_index+=1
        )
        {
            const auto code_point = glyph_index + start_codepoint;
            LoadedGlyph glyph;
            glyph.image.SetupWithAlphaSupport(8, 8, 0);

            for(int y = 0; y < 8; y += 1)
            for(int x = 0; x < 8; x += 1)
            {
                // extract pixel from character
                // glyph is defined in "y down" order, fix by inverting sample point on y
                bool pixel = 0 != (glyphs[glyph_index][7 - y] & 1 << x);
                if(pixel)
                {
                    // glyph.image.SetPixel(x, y, Color::White);
                    DrawSquare(&glyph.image, {Color::White}, x, y, 1);
                }
            }

            glyph.size = glyph.image.GetHeight();
            glyph.bearing_y = glyph.image.GetHeight() + 0;
            glyph.bearing_x = 0;
            glyph.advance = glyph.image.GetWidth() + 0;
            glyph.code_point= code_point;
            glyph.valid = true;
            font.codepoint_to_glyph[code_point] = glyph;
        }

        return font;
    }


    LoadedFont
    LoadCharactersFromBuiltin13()
    {
        LoadedFont font;
        font.line_height = 13;

        for(unsigned int codepoint=32; codepoint < 127; codepoint+=1)
        {
            LoadedGlyph glyph;
            glyph.image.SetupWithAlphaSupport(8, 13, 0);
            
            const auto glyph_index = codepoint - 32;

            for(int y = 0; y < 13; y += 1)
            for(int x = 0; x < 8; x += 1)
            {
                const bool pixel = 0 !=
                (
                    FONT8X13_RASTERS[glyph_index * 13 + y] & (1 << (8-x))
                );
                if(pixel)
                {
                    DrawSquare(&glyph.image, {Color::White}, x, y, 1);
                }
            }

            glyph.size = glyph.image.GetHeight();
            glyph.bearing_y = glyph.image.GetHeight() + 0;
            glyph.bearing_x = 0;
            glyph.advance = glyph.image.GetWidth() + 0;
            glyph.code_point= codepoint;
            glyph.valid = true;
            font.codepoint_to_glyph[codepoint] = glyph;
        }

        return font;
    }


    LoadedFont
    LoadCharactersFromBuiltin8()
    {
        LoadedFont font;
        // todo(Gustav): Add more characters
        font.CombineWith
        (
            GetCharacterFromBuiltin8(0x0000, 0x007F, font8x8_basic)
        );
        return font;
    }


    LoadedFont
    GetCharactersFromFont
    (
        vfs::FileSystem* file_system,
        const vfs::FilePath& font_file,
        unsigned int font_size,
        const std::string& chars
    )
    {
        auto file_memory = file_system->ReadFile(font_file);

        if(file_memory == nullptr)
        {
            LOG_ERROR("Unable to open {0}", font_file);
            return LoadedFont{}; 
        }

        return GetCharactersFromFont
        (
            file_memory,
            font_size,
            chars
        );
    }


    LoadedFont
    GetCharactersFromFont
    (
        std::shared_ptr<MemoryChunk> file_memory,
        unsigned int font_size,
        const std::string& chars
    )
    {
        auto created_lib = FreetypeLibrary::Create();
        if(created_lib.has_value() == false) { return LoadedFont{}; }
        auto lib = std::move(*created_lib);

        auto loaded_face = FreetypeFace::Create
        (
            &lib,
            file_memory,
            font_size
        );
        if(loaded_face.has_value() == false) { return LoadedFont{}; }
        auto f = std::move(*loaded_face);

        std::vector<unsigned int> code_points;
        Utf8ToCodepoints
        (
            chars,
            [&](unsigned int cp){code_points.emplace_back(cp);}
        );

        LoadedFont fontchars {};
        for(char code_point: code_points)
        {
            LoadedGlyph cc = f.LoadGlyph(code_point);
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
            int dx = delta.x >> 6;
            if(dx != 0)
            {
                fontchars.kerning.insert
                (
                    KerningMap::value_type
                    (
                        KerningMap::key_type(previous, current),
                        dx * scale
                    )
                );
            }
        }

        return fontchars;
    }


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
    )
    {
        const auto loaded = LoadImage
        (
            fs,
            image_file,
            AlphaLoad::Keep
        );

        if(loaded.error.empty() == false)
        {
            LOG_ERROR("Failed to load font image {}", image_file);
            return LoadedFont{};
        }

        return GetCharactersFromSingleImage
        (
            loaded.image,
            image_alias,
            image_scale,
            image_bearing_x,
            image_bearing_y,
            image_advance
        );
    }


    LoadedFont
    GetCharactersFromSingleImage
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
        glyph.size = s * image.GetHeight();
        glyph.bearing_y = s * image.GetHeight() + image_bearing_y;
        glyph.bearing_x = image_bearing_x;
        glyph.advance = s * image.GetWidth() + image_advance;
        glyph.code_point= font.NewPrivateUse(image_alias);
        // todo(Gustav): add ability to clip image
        glyph.image = image;
        font.codepoint_to_glyph[glyph.code_point] = glyph;

        return font;
    }

}

