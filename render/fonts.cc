#include "render/fonts.h"

#include "render/texture.h"
#include "core/log.h"
#include "core/proto.h"
#include "core/draw.h"
#include "core/textparser.h"

#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include "core/assert.h"
#include "core/noncopyable.h"
#include "core/image.h"
#include <iostream>

#include "gaf_font.h"

#include "render/spriterender.h"
#include "render/texturecache.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace euphoria::convert;

namespace euphoria::render
{
    LOG_SPECIFY_DEFAULT_LOGGER("core.font")

    namespace
    {
        void
        Error(FT_Error err)
        {
            if (err == 0)
            {
                return;
            }
            std::cerr << "FONT Error: " << err << "\n";
        }

        void
        ErrorNoThrow(FT_Error err)
        {
            if (err == 0)
            {
                return;
            }
            std::cerr << "FONT Error: " << err << "\n";
        }

        std::string
        ConvertCharToIndex(char c)
        {
            return std::string {c};
        }
    }  // namespace

    struct FreetypeLibrary
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
        std::string c;
        int         bearing_x {0};
        int         bearing_y {0};
        int         advance {0};
        core::Image image;
        float       size;
    };

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
            if (error != 0)
            {
                std::cerr << "Failed to get char\n";
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
            if (slot->bitmap.width == 0)
            {
                ch.image.MakeInvalid();
            }
            else
            {
                const bool upside_down = slot->bitmap.pitch > 0;

                ch.image.SetupWithAlphaSupport(
                        slot->bitmap.width, slot->bitmap.rows);
                auto* buffer = slot->bitmap.buffer;
                for (int y = 0; y < ch.image.GetHeight(); y += 1)
                {
                    for (int x = 0; x < ch.image.GetWidth(); x += 1)
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

    Glyph::Glyph(
            const core::Rectf& sprite,
            const core::Rectf& texture,
            const std::string& ch,
            float              ad)
        : sprite_rect(sprite), texture_rect(texture), c(ch), advance(ad)
    {}

    // represent a loaded font (or a part), but it not yet been converted
    // into a renderable data and texture.
    struct LoadedFont
    {
        std::vector<LoadedGlyph> chars;
        KerningMap               kerning;

        void
        CombineWith(const LoadedFont& fc)
        {
            for (const auto& c: fc.chars)
            {
                chars.push_back(c);
            }

            for (const auto& e: fc.kerning)
            {
                const auto found = kerning.find(e.first);
                if (found == kerning.end())
                {
                    kerning.insert(e);
                }
                else
                {
                    LOG_ERROR("Multiple kernings found when trying to combine");
                }
            }
        }
    };

    LoadedFont
    GetCharactersFromSingleImage(
            core::vfs::FileSystem*   fs,
            const font::SingleImage& img)
    {
        LoadedFont font;

        core::ImageLoadResult loaded
                = core::LoadImage(fs, img.file, core::AlphaLoad::Keep);
        if (loaded.error.empty())
        {
            const auto  s = 1 / img.scale;
            LoadedGlyph glyph;
            glyph.size      = s * loaded.image.GetHeight();
            glyph.bearing_y = s * loaded.image.GetHeight() + img.bearing_y;
            glyph.bearing_x = img.bearing_x;
            glyph.advance   = s * loaded.image.GetWidth() + img.advance;
            glyph.c         = img.alias;
            // todo: add ability to clip image
            glyph.image = loaded.image;
            font.chars.emplace_back(glyph);
        }

        return font;
    }

    LoadedFont
    GetCharacterFromBuiltin(const std::string& text, int s)
    {
        LoadedFont font;

        const int size = 8 * s;

        LoadedGlyph glyph;
        glyph.image.SetupWithAlphaSupport(size, size, 0);

        core::Draw {&glyph.image}.Text(
                core::vec2i::Zero(), text, core::Rgbi {255}, s);

        glyph.size      = glyph.image.GetHeight();
        glyph.bearing_y = glyph.image.GetHeight() + 0;
        glyph.bearing_x = 0;
        glyph.advance   = glyph.image.GetWidth() + 0;
        glyph.c         = text;
        font.chars.emplace_back(glyph);

        return font;
    }

    void
    LoadCharactersFromBuiltin(LoadedFont* font, int scale)
    {
        for (int c = ' '; c < 128; c += 1)
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
        for (char c: chars)
        {
            LoadedGlyph cc = f.LoadGlyph(c);
            if (!cc.valid)
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

        if (use_kerning == 1)
        {
            for (const char previous: chars)
            {
                for (const char current: chars)
                {
                    if (previous == current)
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
                    if (dx != 0)
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

    std::pair<core::Rectf, core::Rectf>
    ConstructCharacterRects(
            const stbrp_rect&  src_rect,
            const LoadedGlyph& src_char,
            int                image_width,
            int                image_height)
    {
        const int vert_left  = src_char.bearing_x;
        const int vert_right = vert_left + src_char.image.GetWidth();
        const int vert_top   = src_char.bearing_y;
        const int vert_bottom
                = vert_top - std::max(1, src_char.image.GetHeight());

        const float iw = image_width;
        const float ih = image_height;

        const stbrp_coord uv_left   = src_rect.x;
        const stbrp_coord uv_right  = uv_left + src_rect.w;
        const stbrp_coord uv_bottom = src_rect.y;
        const stbrp_coord uv_top
                = uv_bottom + std::max(static_cast<stbrp_coord>(1), src_rect.h);

        // todo: add ability to be a quad for tighter fit
        ASSERTX(vert_top > vert_bottom, vert_top, vert_bottom, src_char.c);
        ASSERTX(uv_top > uv_bottom, uv_top, uv_bottom, src_char.c);
        const auto sprite = core::Rectf::FromLeftRightTopBottom(
                vert_left, vert_right, vert_top, vert_bottom);
        const auto texture = core::Rectf::FromLeftRightTopBottom(
                uv_left / iw, uv_right / iw, uv_top / ih, uv_bottom / ih);

        const float scale = 1 / src_char.size;
        return std::make_pair(sprite.ScaleCopy(scale, scale), texture);
    }

    ////////////////////////////////////////////////////////////////////////////////

    Font::Font(
            core::vfs::FileSystem* fs,
            TextureCache*          cache,
            const std::string&     font_file)
    {
        const int texture_width  = 512;
        const int texture_height = 512;

        background = cache->GetTexture("img-plain/white");

        LoadedFont fontchars;
        font::Root font_root;

        std::string error = core::LoadProtoJson(fs, &font_root, font_file);
        if (!error.empty())
        {
            LOG_ERROR("Failed to load " << font_file << ": " << error);
        }
        for (const auto& source: font_root.sources)
        {
            if (source.font)
            {
                const font::FontFile& font = *source.font;
                fontchars.CombineWith(GetCharactersFromFont(
                        font.file, font_root.size, font.characters));
            }
            if (source.image)
            {
                const font::SingleImage& image = *source.image;
                fontchars.CombineWith(GetCharactersFromSingleImage(fs, image));
            }
            if (source.builtin)
            {
                LoadCharactersFromBuiltin(&fontchars, source.builtin->scale);
            }
            // todo: add more sources, built in image font or images
        }

        // the half margin between glyphs in the final texture
        const int half_margin = 1;

        // pack char textures to a single texture
        const int               num_rects = fontchars.chars.size();
        std::vector<stbrp_rect> rects(num_rects);
        for (int i = 0; i < num_rects; ++i)
        {
            stbrp_rect& r = rects[i];
            r.id          = i;
            r.w = fontchars.chars[i].image.GetWidth() + half_margin * 2;
            r.h = fontchars.chars[i].image.GetHeight() + half_margin * 2;
        }
        stbrp_context           context {};
        const int               num_nodes = texture_width;
        std::vector<stbrp_node> nodes(num_nodes);
        stbrp_init_target(
                &context, texture_width, texture_height, &nodes[0], num_nodes);
        stbrp_pack_rects(&context, &rects[0], num_rects);

        CharDataMap map;
        core::Image image;
        image.SetupWithAlphaSupport(texture_width, texture_height);
        for (int i = 0; i < num_rects; ++i)
        {
            const stbrp_rect& src_rect = rects[i];
            if (src_rect.was_packed == 0)
            {
                std::cerr << "Failed to pack\n";
                continue;
            }
            const LoadedGlyph& src_char = fontchars.chars[src_rect.id];
            core::Draw {&image}.PasteImage(
                    core::vec2i {src_rect.x + half_margin,
                                 src_rect.y + half_margin},
                    src_char.image);
            const auto rects = ConstructCharacterRects(
                    src_rect, src_char, texture_width, texture_height);

            std::shared_ptr<Glyph> dest(new Glyph(
                    rects.first,
                    rects.second,
                    src_char.c,
                    src_char.advance / src_char.size));
            map.insert(CharDataMap::value_type(dest->c, dest));
        }

        // for debug
        // fs->WriteFile("charmap.png", image.Write(ImageWriteFormat::PNG));

        // load pixels into texture
        kerning_ = fontchars.kerning;
        chars_   = map;
        Texture2dLoadData load_data;
        texture_ = std::make_unique<Texture2d>();
        texture_->LoadFromImage(
                image, core::AlphaLoad::Keep, Texture2dLoadData());
    }

    void
    Font::DrawBackground(
            SpriteRenderer*    renderer,
            float              alpha,
            const core::Rectf& where) const
    {
        renderer->DrawRect(
                *background,
                where,
                core::Rectf::FromWidthHeight(1, 1),
                0.0_rad,
                core::scale2f {0, 0},
                core::Rgba {core::Color::Black, alpha});
    }

    TextDrawCommand::TextDrawCommand(
            const Texture2d*   texture,
            const core::Rectf& sprite_rect,
            const core::Rectf& texture_rect,
            bool               hi)
        : texture(texture)
        , sprite_rect(sprite_rect)
        , texture_rect(texture_rect)
        , hi(hi)
    {}

    void
    TextDrawCommandList::Add(
            const Texture2d*   texture,
            const core::Rectf& sprite_rect,
            const core::Rectf& texture_rect,
            bool               hi)
    {
        commands.emplace_back(texture, sprite_rect, texture_rect, hi);
    }

    void
    TextDrawCommandList::Draw(
            SpriteRenderer*    renderer,
            const core::vec2f& start_position,
            const core::Rgb&   base_color,
            const core::Rgb&   hi_color)
    {
        for (const auto& cmd: commands)
        {
            const auto tint = cmd.hi ? hi_color : base_color;
            renderer->DrawRect(
                    *cmd.texture,
                    cmd.sprite_rect.OffsetCopy(start_position),
                    cmd.texture_rect,
                    0.0_rad,
                    core::scale2f {0.5f, 0.5f},
                    core::Rgba {tint});
        }
    }

    struct ParsedTextCompileVisitor : public core::textparser::Visitor
    {
        const Texture2d*   texture_;
        const CharDataMap* chars_;
        const KerningMap*  kerning_;
        float              size;
        bool               apply_highlight;
        core::vec2f        position;  // todo: rename to offset
        std::string        last_char_index;

        // return value
        TextDrawCommandList* list;

        ParsedTextCompileVisitor(
                const Texture2d*     texture,
                const CharDataMap*   chars,
                const KerningMap*    kerning,
                float                size,
                TextDrawCommandList* list)
            : texture_(texture)
            , chars_(chars)
            , kerning_(kerning)
            , size(size)
            , apply_highlight(false)
            , position(0, 0)
            , list(list)
        {}

        void
        OnText(const std::string& text) override
        {
            for (char c: text)
            {
                const std::string char_index = ConvertCharToIndex(c);
                AddCharIndex(char_index);
            }
        }

        void
        OnImage(const std::string& image) override
        {
            AddCharIndex(image);
        }

        void
        OnBegin() override
        {
            apply_highlight = true;
        }

        void
        OnEnd() override
        {
            apply_highlight = false;
        }

        void
        AddCharIndex(const std::string& char_index)
        {
            if (char_index == "\n")
            {
                position.x = 0;
                position.y -= size;
            }
            else
            {
                auto it = chars_->find(char_index);
                if (it == chars_->end())
                {
                    LOG_ERROR("Failed to print '" << char_index << "'");
                    return;
                }
                std::shared_ptr<Glyph> ch = it->second;

                list->Add(
                        texture_,
                        ch->sprite_rect.ScaleCopy(size, size)
                                .OffsetCopy(position),
                        ch->texture_rect,
                        apply_highlight);

                auto kerning = kerning_->find(
                        std::make_pair(last_char_index, char_index));
                int the_kerning
                        = kerning == kerning_->end() ? 0 : kerning->second;
                position.x += (ch->advance + the_kerning) * size;
            }
            last_char_index = char_index;
        }
    };

    TextDrawCommandList
    Font::CompileList(const core::ParsedText& text, float size) const
    {
        TextDrawCommandList list;

        ParsedTextCompileVisitor vis {
                texture_.get(), &chars_, &kerning_, size, &list};
        text.Visit(&vis);

        return list;
    }

    core::Rectf
    TextDrawCommandList::GetExtents() const
    {
        core::Rectf ret;
        for (const auto& cmd: commands)
        {
            ret.Include(cmd.sprite_rect);
        }
        return ret;
    }

    Text::Text(Font* font)
        : font_(font)
        , size_(12.0f)
        , alignment_(Align::BASELINE_LEFT)
        , use_background_(false)
        , background_alpha_(0.0f)
        , dirty(true)
    {}

    Text::~Text() = default;

    void
    Text::SetText(const core::ParsedText& str)
    {
        text_ = str;
        dirty = true;
    }

    void
    Text::SetBackground(bool use_background, float alpha)
    {
        use_background_   = use_background;
        background_alpha_ = alpha;
    }

    void
    Text::SetAlignment(Align alignment)
    {
        alignment_ = alignment;
    }

    void
    Text::SetSize(float new_size)
    {
        if (size_ != new_size)
        {
            size_ = new_size;
            dirty = true;
        }
    }

    core::vec2f
    GetOffset(Align alignment, const core::Rectf& extent)
    {
        // todo: test this more
        const auto middle = -(extent.left + extent.right) / 2;
        const auto right  = -extent.right;
        const auto top    = extent.top;
        const auto bottom = -extent.bottom;

        switch (alignment)
        {
        case Align::TOP_LEFT: return core::vec2f(0.0f, top);
        case Align::TOP_CENTER: return core::vec2f(middle, top);
        case Align::TOP_RIGHT: return core::vec2f(right, top);
        case Align::BASELINE_LEFT: return core::vec2f(0.0f, 0.0f);
        case Align::BASELINE_CENTER: return core::vec2f(middle, 0.0f);
        case Align::BASELINE_RIGHT: return core::vec2f(right, 0.0f);
        case Align::BOTTOM_LEFT: return core::vec2f(0.0f, bottom);
        case Align::BOTTOM_CENTER: return core::vec2f(middle, bottom);
        case Align::BOTTOM_RIGHT: return core::vec2f(right, bottom);
        default: DIE("Unhandled case"); return core::vec2f(0.0f, 0.0f);
        }
    }

    void
    Text::Draw(
            SpriteRenderer*    renderer,
            const core::vec2f& p,
            const core::Rgb&   base_hi_color) const
    {
        Draw(renderer, p, base_hi_color, base_hi_color);
    }

    void
    Text::Draw(
            SpriteRenderer*    renderer,
            const core::vec2f& p,
            const core::Rgb&   base_color,
            const core::Rgb&   hi_color) const
    {
        Compile();
        ASSERT(!dirty);

        if (font_ == nullptr)
        {
            return;
        }
        const auto e   = GetExtents();
        const auto off = GetOffset(alignment_, e);
        if (use_background_)
        {
            font_->DrawBackground(
                    renderer,
                    background_alpha_,
                    e.ExtendCopy(5.0f).OffsetCopy(p + off));
        }

        commands.Draw(renderer, p + off, base_color, hi_color);
    }

    void
    Text::Compile() const
    {
        if (dirty)
        {
            dirty    = false;
            commands = font_->CompileList(text_, size_);
        }
    }

    core::Rectf
    Text::GetExtents() const
    {
        Compile();
        ASSERT(!dirty);
        return commands.GetExtents();
    }

}  // namespace euphoria::render
