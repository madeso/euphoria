#include "render/font.h"


#include <memory>
#include <map>
#include <algorithm>
#include <iostream>

#include "stb_rect_pack.h"

#include "log/log.h"
#include "core/proto.h"
#include "core/image_draw.h"
#include "core/ui_text.h"
#include "core/utf8.h"
#include "assert/assert.h"
#include "core/image.h"
#include "core/vfs_path.h"
#include "core/stdutils.h"
#include "core/stringmerger.h"

#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

#include "gaf_font.h"
#include "gaf_rapidjson_font.h"

using namespace euphoria::convert;


namespace euphoria::render
{
    Glyph::Glyph
    (
        const core::Rectf& sprite,
        const core::Rectf& texture,
        int ch,
        float ad
    )
        : sprite_rect(sprite)
        , texture_rect(texture)
        , code_point(ch)
        , advance(ad)
    {
    }


    core::LoadedFont
    get_characters_from_single_image
    (
        core::vfs::FileSystem* fs,
        const font::SingleImage& img
    )
    {
        const auto image_file = core::vfs::FilePath::from_script(img.file);

        if(image_file.has_value() == false)
        {
            LOG_WARN("Invalid path {0}", img.file);
            return core::LoadedFont{};
        }

        return core::get_characters_from_single_image
        (
            fs,
            image_file.value(),
            img.alias,
            img.scale,
            core::c_int_to_float(img.bearing_x),
            core::c_int_to_float(img.bearing_y),
            core::c_int_to_float(img.advance)
        );
    }


    std::pair<core::Rectf, core::Rectf>
    construct_character_rects
    (
        const stbrp_rect& src_rect,
        const core::LoadedGlyph& src_char,
        int image_width,
        int image_height
    )
    {
        const int vert_left = src_char.bearing_x;
        const int vert_right = vert_left + src_char.image.width;
        const int vert_top = src_char.bearing_y;
        const int vert_bottom = vert_top - std::max(1, src_char.image.height);

        const stbrp_coord uv_left = src_rect.x;
        const stbrp_coord uv_right = uv_left + src_rect.w;
        const stbrp_coord uv_bottom = src_rect.y;
        const stbrp_coord uv_top = uv_bottom + std::max(1, src_rect.h);

        // todo(Gustav): add ability to be a quad for tighter fit
        ASSERTX(vert_top > vert_bottom, vert_top, vert_bottom, src_char.code_point);
        ASSERTX(uv_top > uv_bottom, uv_top, uv_bottom, src_char.code_point);

        const auto iw = static_cast<float>(image_width);
        const auto ih = static_cast<float>(image_height);

        const auto sprite = core::Rectf::from_left_right_top_bottom
        (
            static_cast<float>(vert_left),
            static_cast<float>(vert_right),
            static_cast<float>(vert_top),
            static_cast<float>(vert_bottom)
        );
        const auto texture = core::Rectf::from_left_right_top_bottom
        (
            static_cast<float>(uv_left) / iw,
            static_cast<float>(uv_right) / iw,
            static_cast<float>(uv_top) / ih,
            static_cast<float>(uv_bottom) / ih
        );

        const float scale = 1.0f / src_char.size;
        return std::make_pair(sprite.scale_copy(scale, scale), texture);
    }


    ////////////////////////////////////////////////////////////////////////////////


    DrawableFont::DrawableFont
    (
        core::vfs::FileSystem* fs,
        TextureCache* cache,
        const core::vfs::FilePath& font_file
    )
    {
        // todo(Gustav): too long, break up
        const int texture_width = 512;
        const int texture_height = 512;

        background = cache->get_texture
        (
            core::vfs::FilePath{"~/img-plain/white"}
        );

        core::LoadedFont fontchars;

        const auto font_root = core::get_default_but_log_errors
        (
            core::read_json_file_to_gaf_struct<font::Root>(fs, font_file, font::ReadJsonRoot)
        );

        // todo(Gustav): handle error better
        if (font_root.sources.empty())
        {
            LOG_ERROR("Missing sources in {}", font_file);
            return;
        }
        
        for(const auto& source: font_root.sources)
        {
            if(source.font)
            {
                const font::FontFile& font = *source.font;

                const auto p = core::vfs::FilePath::from_script(font.file);
                if(p.has_value() == false)
                {
                    LOG_ERROR("Invalid path {0}", font.file);
                    return;
                }

                fontchars.combine_with
                (
                    core::get_characters_from_font
                    (
                        fs,
                        *p,
                        font_root.size,
                        font.characters
                    )
                );
            }
            if(source.image)
            {
                const font::SingleImage& image = *source.image;
                const auto image_font = get_characters_from_single_image(fs, image);
                fontchars.combine_with(image_font);
            }
            if(source.builtin8)
            {
                fontchars.combine_with(core::load_characters_from_builtin8());
            }
            if(source.builtin13)
            {
                fontchars.combine_with(core::load_characters_from_builtin13());
            }
            // todo(Gustav): add more sources, built in image font or images
        }

        // the half margin between glyphs in the final texture
        const int half_margin = 1;

        // todo(Gustav): use core/pack.h instead

        // pack char textures to a single texture
        const int num_rects = core::c_sizet_to_int(fontchars.codepoint_to_glyph.size());
        std::vector<stbrp_rect> packed_rects(num_rects);
        std::map<int, int> id_to_codepoint;
        {
            int index = 0;
            for(const auto& [codepoint, glyph]: fontchars.codepoint_to_glyph)
            {
                stbrp_rect& r = packed_rects[index];
                r.id = index;
                r.w = glyph.image.width + half_margin * 2;
                r.h = glyph.image.height + half_margin * 2;
                id_to_codepoint[index] = codepoint;
                index +=1;
            }
        }
        stbrp_context context {};
        const int num_nodes = texture_width;
        std::vector<stbrp_node> nodes(num_nodes);
        stbrp_init_target(&context, texture_width, texture_height, &nodes[0], num_nodes);
        stbrp_pack_rects(&context, &packed_rects[0], num_rects);

        CharToGlyphMap map;
        core::Image image;
        image.setup_with_alpha_support(texture_width, texture_height);
        for(int i = 0; i < num_rects; ++i)
        {
            const stbrp_rect& src_rect = packed_rects[i];
            if(src_rect.was_packed == 0)
            {
                LOG_ERROR("Failed to pack");
                continue;
            }
            const auto& src_char = fontchars.codepoint_to_glyph[id_to_codepoint[src_rect.id]];
            core::paste_image
            (
                &image,
                core::vec2i
                {
                    src_rect.x + half_margin,
                    src_rect.y + half_margin
                },
                src_char.image
            );
            const auto sprite_and_texture_rects = construct_character_rects
            (
                src_rect,
                src_char,
                texture_width,
                texture_height
            );

            std::shared_ptr<Glyph> dest
            (
                new Glyph
                (
                    sprite_and_texture_rects.first,
                    sprite_and_texture_rects.second,
                    src_char.code_point,
                    static_cast<float>(src_char.advance) / src_char.size
                )
            );
            map.insert(CharToGlyphMap::value_type(dest->code_point, dest));
        }

        // for debug
        // fs->WriteFile("charmap.png", image.Write(ImageWriteFormat::PNG));

        // load pixels into texture
        private_use_aliases = fontchars.private_use_aliases;
        kernings = fontchars.kerning;
        char_to_glyph = map;
        Texture2dLoadData load_data;
        texture = std::make_unique<Texture2>();
        texture->load_from_image
        (
                image,
                core::AlphaLoad::keep, Texture2dLoadData()
        );
        line_height = static_cast<float>(fontchars.line_height);
    }


    void
    DrawableFont::draw_background
    (
        SpriteRenderer* renderer,
        float alpha,
        const core::Rectf& where
    ) const
    {
        renderer->draw_rect
        (
            *background,
            where,
            core::Rectf::from_width_height(1, 1),
            0.0_rad,
            core::Scale2f{0, 0},
            core::rgba{core::NamedColor::black, alpha}
        );
    }


    TextDrawCommand::TextDrawCommand
    (
        const Texture2* atexture,
        const core::Rectf& asprite_rect,
        const core::Rectf& atexture_rect,
        bool ahi
    )
        : texture(atexture)
        , sprite_rect(asprite_rect)
        , texture_rect(atexture_rect)
        , hi(ahi)
    {
    }


    void
    ListOfTextDrawCommands::add
    (
        const Texture2* texture,
        const core::Rectf& sprite_rect,
        const core::Rectf& texture_rect,
        bool hi
    )
    {
        commands.emplace_back(texture, sprite_rect, texture_rect, hi);
    }


    void
    ListOfTextDrawCommands::draw
    (
        SpriteRenderer* renderer,
        const core::vec2f& start_position,
        const core::rgb& base_color,
        const core::rgb& hi_color
    )
    {
        for(const auto& cmd: commands)
        {
            const auto tint = cmd.hi ? hi_color : base_color;
            renderer->draw_rect
            (
                *cmd.texture,
                cmd.sprite_rect.offset_copy(start_position),
                cmd.texture_rect,
                0.0_rad,
                core::Scale2f{0.5f, 0.5f},
                core::rgba{tint}
            );
        }
    }


    struct UiTextCompileVisitor : public core::textparser::Visitor
    {
        const DrawableFont& font;
        float size;
        bool apply_highlight;
        core::vec2f position; // todo(Gustav): rename to offset
        int last_char_index = 0;

        // return value
        ListOfTextDrawCommands* list;

        UiTextCompileVisitor
        (
            const DrawableFont& f,
            float s,
            ListOfTextDrawCommands* li
        )
            : font(f)
            , size(s)
            , apply_highlight(false)
            , position(0, 0)
            , list(li)
        {
        }


        void
        on_text(const std::string& text) override
        {
            core::calc_utf8_to_codepoints
            (
                text,
                [this](int cp)
                {
                    add_char_index(cp);
                }
            );
        }


        void
        on_image(const std::string& image) override
        {
            // todo(Gustav): handle invalid font alias
            auto found = font.private_use_aliases.find(image);
            if(found == font.private_use_aliases.end())
            {
                LOG_ERROR
                (
                    "Unable to find image {0}, could be {1}",
                    image,
                    core::string_mergers::english_or.merge(core::get_keys(font.private_use_aliases))
                );
                return;
            }
            add_char_index(found->second);
        }


        void
        on_begin() override
        {
            apply_highlight = true;
        }


        void
        on_end() override
        {
            apply_highlight = false;
        }


        void
        add_char_index(int code_point)
        {
            if(code_point == '\n')
            {
                position.x = 0;
                position.y -= size*font.line_height;
            }
            else
            {
                auto it = font.char_to_glyph.find(code_point);
                if(it == font.char_to_glyph.end())
                {
                    LOG_ERROR("Failed to print '{0}'", code_point);
                    return;
                }
                std::shared_ptr<Glyph> ch = it->second;

                list->add
                (
                    font.texture.get(),
                    ch->sprite_rect.scale_copy(size, size).offset_copy(position),
                    ch->texture_rect,
                    apply_highlight
                );

                const auto kerning = font.kernings.find
                (
                    std::make_pair
                    (
                        last_char_index,
                        code_point
                    )
                );
                const float the_kerning = kerning == font.kernings.end()
                    ? 0.0f
                    : kerning->second
                    ;
                position.x += (ch->advance + the_kerning) * size;
            }
            last_char_index = code_point;
        }
    };


    ListOfTextDrawCommands
    DrawableFont::compile_list(const core::UiText& text, float size) const
    {
        ListOfTextDrawCommands list;

        UiTextCompileVisitor vis {*this, size, &list};
        text.accept(&vis);

        return list;
    }


    core::Rectf
    ListOfTextDrawCommands::get_extents() const
    {
        core::Rectf ret;
        for(const auto& cmd: commands)
        {
            ret.include(cmd.sprite_rect);
        }
        return ret;
    }


    DrawableText::DrawableText(DrawableFont* the_font)
        : font(the_font)
        , size(12.0f)
        , alignment(Align::baseline_left)
        , use_background(false)
        , background_alpha(0.0f)
        , is_dirty(true)
    {
    }


    DrawableText::~DrawableText() = default;


    void
    DrawableText::set_text(const core::UiText& new_text)
    {
        text = new_text;
        is_dirty = true;
    }


    void
    DrawableText::set_background(bool new_use_background, float new_alpha)
    {
        use_background = new_use_background;
        background_alpha = new_alpha;
    }


    void
    DrawableText::set_alignment(Align new_alignment)
    {
        alignment = new_alignment;
    }


    void
    DrawableText::set_size(float new_size)
    {
        if(size != new_size)
        {
            size = new_size;
            is_dirty = true;
        }
    }


    core::vec2f
    get_offset(Align alignment, const core::Rectf& extent)
    {
        // todo(Gustav): test this more
        const auto middle = -(extent.left + extent.right) / 2;
        const auto right = -extent.right;
        const auto top = extent.top;
        const auto bottom = -extent.bottom;

        switch(alignment)
        {
        case Align::top_left: return core::vec2f(0.0f, top);
        case Align::top_center: return core::vec2f(middle, top);
        case Align::top_right: return core::vec2f(right, top);
        case Align::baseline_left: return core::vec2f(0.0f, 0.0f);
        case Align::baseline_center: return core::vec2f(middle, 0.0f);
        case Align::baseline_right: return core::vec2f(right, 0.0f);
        case Align::bottom_left: return core::vec2f(0.0f, bottom);
        case Align::bottom_center: return core::vec2f(middle, bottom);
        case Align::bottom_right: return core::vec2f(right, bottom);
        default: DIE("Unhandled case"); return core::vec2f(0.0f, 0.0f);
        }
    }


    void
    DrawableText::draw
    (
        SpriteRenderer* renderer,
        const core::vec2f& p,
        const core::rgb& base_hi_color
    ) const
    {
        draw(renderer, p, base_hi_color, base_hi_color);
    }


    void
    DrawableText::draw
    (
        SpriteRenderer* renderer,
        const core::vec2f& p,
        const core::rgb& base_color,
        const core::rgb& hi_color
    ) const
    {
        compile();
        ASSERT(!is_dirty);

        if(font == nullptr)
        {
            return;
        }
        const auto e = get_extents();
        const auto off = get_offset(alignment, e);
        if(use_background)
        {
            font->draw_background
            (
                renderer,
                background_alpha,
                e.extend_copy(5.0f).offset_copy(p + off)
            );
        }

        commands.draw(renderer, p + off, base_color, hi_color);
    }


    void
    DrawableText::compile() const
    {
        if(is_dirty)
        {
            is_dirty = false;
            commands = font->compile_list(text, size);
        }
    }


    core::Rectf
    DrawableText::get_extents() const
    {
        compile();
        ASSERT(!is_dirty);
        return commands.get_extents();
    }
}
