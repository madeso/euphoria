#include "render/fonts.h"

#include "render/texture.h"
#include "core/log.h"
#include "core/proto.h"
#include "core/image_draw.h"
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

using namespace euphoria::convert;

namespace euphoria::render
{
    LOG_SPECIFY_DEFAULT_LOGGER("render.font")

    Glyph::Glyph(
            const core::Rectf& sprite,
            const core::Rectf& texture,
            const std::string& ch,
            float              ad)
        : sprite_rect(sprite), texture_rect(texture), c(ch), advance(ad)
    {}

    core::LoadedFont
    GetCharactersFromSingleImage(
            core::vfs::FileSystem*   fs,
            const font::SingleImage& img)
    {
        core::LoadedFont font;

        core::ImageLoadResult loaded
                = core::LoadImage(fs, img.file, core::AlphaLoad::Keep);
        if(loaded.error.empty())
        {
            const auto  s = 1 / img.scale;
            core::LoadedGlyph glyph;
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


    std::pair<core::Rectf, core::Rectf>
    ConstructCharacterRects(
            const stbrp_rect&  src_rect,
            const core::LoadedGlyph& src_char,
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

        core::LoadedFont fontchars;
        font::Root font_root;

        std::string error = core::LoadProtoJson(fs, &font_root, font_file);
        if(!error.empty())
        {
            LOG_ERROR("Failed to load " << font_file << ": " << error);
        }
        for(const auto& source: font_root.sources)
        {
            if(source.font)
            {
                const font::FontFile& font = *source.font;
                fontchars.CombineWith(core::GetCharactersFromFont(
                        font.file, font_root.size, font.characters));
            }
            if(source.image)
            {
                const font::SingleImage& image = *source.image;
                fontchars.CombineWith(GetCharactersFromSingleImage(fs, image));
            }
            if(source.builtin)
            {
                LoadCharactersFromBuiltin(&fontchars, source.builtin->scale);
            }
            // todo: add more sources, built in image font or images
        }

        // the half margin between glyphs in the final texture
        const int half_margin = 1;

        // pack char textures to a single texture
        const int               num_rects = fontchars.chars.size();
        std::vector<stbrp_rect> packed_rects(num_rects);
        for(int i = 0; i < num_rects; ++i)
        {
            stbrp_rect& r = packed_rects[i];
            r.id          = i;
            r.w = fontchars.chars[i].image.GetWidth() + half_margin * 2;
            r.h = fontchars.chars[i].image.GetHeight() + half_margin * 2;
        }
        stbrp_context           context {};
        const int               num_nodes = texture_width;
        std::vector<stbrp_node> nodes(num_nodes);
        stbrp_init_target(
                &context, texture_width, texture_height, &nodes[0], num_nodes);
        stbrp_pack_rects(&context, &packed_rects[0], num_rects);

        CharDataMap map;
        core::Image image;
        image.SetupWithAlphaSupport(texture_width, texture_height);
        for(int i = 0; i < num_rects; ++i)
        {
            const stbrp_rect& src_rect = packed_rects[i];
            if(src_rect.was_packed == 0)
            {
                LOG_ERROR("Failed to pack");
                continue;
            }
            const auto& src_char = fontchars.chars[src_rect.id];
            core::PasteImage(
                    &image,
                    core::vec2i {src_rect.x + half_margin,
                                 src_rect.y + half_margin},
                    src_char.image);
            const auto sprite_and_texture_rects = ConstructCharacterRects(
                    src_rect, src_char, texture_width, texture_height);

            std::shared_ptr<Glyph> dest(new Glyph(
                    sprite_and_texture_rects.first,
                    sprite_and_texture_rects.second,
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
        for(const auto& cmd: commands)
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
        const core::KerningMap*  kerning_;
        float              size;
        bool               apply_highlight;
        core::vec2f        position;  // todo: rename to offset
        std::string        last_char_index;

        // return value
        TextDrawCommandList* list;

        ParsedTextCompileVisitor(
                const Texture2d*     texture,
                const CharDataMap*   chars,
                const core::KerningMap*    kerning,
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
            for(char c: text)
            {
                const std::string char_index = core::ConvertCharToIndex(c);
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
            if(char_index == "\n")
            {
                position.x = 0;
                position.y -= size;
            }
            else
            {
                auto it = chars_->find(char_index);
                if(it == chars_->end())
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
        for(const auto& cmd: commands)
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
        if(size_ != new_size)
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

        switch(alignment)
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

        if(font_ == nullptr)
        {
            return;
        }
        const auto e   = GetExtents();
        const auto off = GetOffset(alignment_, e);
        if(use_background_)
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
        if(dirty)
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
