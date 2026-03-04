#include "render/font.h"


#include <memory>
#include <map>
#include <algorithm>
#include <iostream>

#include "stb_rect_pack.h"

#include "base/cint.h"
#include "log/log.h"
// #include "core/image_draw.h"
#include "render/utf8.h"
#include "assert/assert.h"
#include "render/image.h"
// #include "render/stdutils.h"
// #include "base/stringmerger.h"

// #include "files/font.h"

#include "render/texture.h"
// #include "render/spriterender.h"
// #include "render/texturecache.h"

#include "canvas.h"
#include "dependency_glad.h"
#include "base/memorychunk.h"

using namespace eu::convert;


namespace eu::render
{
    Glyph::Glyph
    (
        const Rect& sprite,
        const Rect& texture,
        int ch,
        float ad
    )
        : sprite_rect(sprite)
        , texture_rect(texture)
        , code_point(ch)
        , advance(ad)
    {
    }


    std::pair<Rect, Rect>
    construct_character_rects
    (
        const stbrp_rect& src_rect,
        const LoadedGlyph& src_char,
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
        const stbrp_coord uv_top = uv_bottom + std::max<int>(1, src_rect.h);

        // todo(Gustav): add ability to be a quad for tighter fit
        ASSERTX(vert_top > vert_bottom, vert_top, vert_bottom, src_char.code_point);
        ASSERTX(uv_top > uv_bottom, uv_top, uv_bottom, src_char.code_point);

        const auto iw = float_from_int(image_width);
        const auto ih = float_from_int(image_height);

        const auto sprite = Rect::from_left_right_top_bottom
        (
            float_from_int(vert_left),
            float_from_int(vert_right),
            float_from_int(vert_top),
            float_from_int(vert_bottom)
        );
        const auto texture = Rect::from_left_right_top_bottom
        (
            float_from_int(uv_left) / iw,
            float_from_int(uv_right) / iw,
            float_from_int(uv_top) / ih,
            float_from_int(uv_bottom) / ih
        );

        // scale to make sure bake resolution is separate from rendering size
        const float scale = 1.0f / src_char.size;
        return std::make_pair(sprite.with_scale(scale, scale), texture);
    }


    ////////////////////////////////////////////////////////////////////////////////


    DrawableFont::DrawableFont
    (
        const MemoryChunk& file_memory
    )
    {
        // todo(Gustav): too long, break up
        const int texture_width = 512;
        const int texture_height = 512;

        LoadedFont fontchars;

        fontchars.combine_with
        (
            get_characters_from_font
            (
                file_memory,
                12,
                // todo(Gustav): use a range instead
                "abcdefghijklmnopqrstuwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-=_+[]{}|;:'\",.<>/?`~ "
            )
        );
        
        
        // const auto image_font = get_characters_from_single_image(fs, image);
        // fontchars.combine_with(image_font);

        // todo(Gustav): add more sources, built in image font or images

        // the half margin between glyphs in the final texture
        const int half_margin = 1;

        // todo(Gustav): use core/pack.h instead

        // pack char textures to a single texture
        const int num_rects = int_from_sizet(fontchars.codepoint_to_glyph.size());
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
        stbrp_init_target(&context, texture_width, texture_height, nodes.data(), num_nodes);
        stbrp_pack_rects(&context, packed_rects.data(), num_rects);

        CharToGlyphMap map;
        Image image;
        image.setup_with_alpha_support(texture_width, texture_height);
        for(int rect_index = 0; rect_index < num_rects; rect_index += 1)
        {
            const stbrp_rect& src_rect = packed_rects[rect_index];
            if(src_rect.was_packed == 0)
            {
                LOG_ERR("Failed to pack");
                continue;
            }
            const auto& src_char = fontchars.codepoint_to_glyph[id_to_codepoint[src_rect.id]];
            if (src_char.image.is_valid() == false)
            {
                LOG_WARN("Glyph for code point {0} has an invalid image, skipping", src_char.code_point);
            }
            else
            {
                paste_image
                (
                    &image,
                    src_rect.x + half_margin,
                    src_rect.y + half_margin,
                    src_char.image
                );
            }
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
        texture = std::make_unique<Texture2d>(SEND_DEBUG_LABEL_MANY("FONT TEXTURE") image.data.data(), GL_RGBA, texture_width, texture_height, TextureEdge::clamp, TextureRenderStyle::linear, Transparency::include, ColorData::color_data);
        line_height = static_cast<float>(fontchars.line_height);
    }


    void
    DrawableFont::draw_background
    (
        SpriteBatch* renderer,
        float alpha,
        const Rect& where
    ) const
    {
        renderer->quad
        (
            std::nullopt,
            where,
            std::nullopt,
            Color{eu::colors::black, alpha}
        );
    }


    TextDrawCommand::TextDrawCommand
    (
        const Texture2d* atexture,
        const Rect& asprite_rect,
        const Rect& atexture_rect,
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
        const Texture2d* texture,
        const Rect& sprite_rect,
        const Rect& texture_rect,
        bool hi
    )
    {
        commands.emplace_back(texture, sprite_rect, texture_rect, hi);
    }


    void
    ListOfTextDrawCommands::draw
    (
        SpriteBatch* renderer,
        const v2& start_position,
        const Rgb& base_color,
        const Rgb& hi_color
    )
    {
        for(const auto& cmd: commands)
        {
            const auto tint = cmd.hi ? hi_color : base_color;
            renderer->quad
            (
                cmd.texture,
                cmd.sprite_rect.with_translate(start_position),
                cmd.texture_rect,
                Color{tint}
            );
        }
    }


    struct UiTextCompileVisitor// : public textparser::Visitor
    {
        const DrawableFont& font;
        float size;
        bool apply_highlight = false;
        v2 position = v2{0, 0}; // todo(Gustav): rename to offset
        int last_char_index = 0;

        // return value
        ListOfTextDrawCommands* list;

        UiTextCompileVisitor
        (
            const DrawableFont& f,
            float s,
            ListOfTextDrawCommands* li
        )
            : font{f}
            , size{s}
            , list{li}
        {
        }


        void
        on_text(const std::string& text)// override
        {
            calc_utf8_to_codepoints
            (
                text,
                [this](int cp)
                {
                    add_char_index(cp);
                }
            );
        }

#if 0
        void
        on_image(const std::string& image) override
        {
            // todo(Gustav): handle invalid font alias
            auto found = font.private_use_aliases.find(image);
            if(found == font.private_use_aliases.end())
            {
                LOG_ERR
                (
                    "Unable to find image {0}, could be {1}",
                    image,
                    string_mergers::english_or.merge(get_keys(font.private_use_aliases))
                );
                return;
            }
            add_char_index(found->second);
        }
#endif


        void
        on_begin()// override
        {
            apply_highlight = true;
        }


        void
        on_end()// override
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
                    LOG_ERR("Failed to print '{0}'", code_point);
                    return;
                }
                std::shared_ptr<Glyph> ch = it->second;

                list->add
                (
                    font.texture.get(),
                    ch->sprite_rect.with_scale(size, size).with_translate(position),
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
    DrawableFont::compile_list(const std::string& text, float size) const
    {
        ListOfTextDrawCommands list;

        UiTextCompileVisitor vis {*this, size, &list};
        // text.accept(&vis);
        vis.on_begin();
        vis.on_text(text);
        vis.on_end();

        return list;
    }


    Rect
    ListOfTextDrawCommands::get_extents() const
    {
        Rect ret;
        for(const auto& cmd: commands)
        {
            ret.extend(cmd.sprite_rect);
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
    DrawableText::set_text(const std::string& new_text)
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


    v2
    get_offset(Align alignment, const Rect& extent)
    {
        // todo(Gustav): test this more
        const auto middle = -(extent.left + extent.right) / 2;
        const auto right = -extent.right;
        const auto top = extent.top;
        const auto bottom = -extent.bottom;

        switch(alignment)
        {
        case Align::top_left: return {0.0f, top};
        case Align::top_center: return {middle, top};
        case Align::top_right: return {right, top};
        case Align::baseline_left: return {0.0f, 0.0f};
        case Align::baseline_center: return {middle, 0.0f};
        case Align::baseline_right: return {right, 0.0f};
        case Align::bottom_left: return {0.0f, bottom};
        case Align::bottom_center: return {middle, bottom};
        case Align::bottom_right: return {right, bottom};
        default: DIE("Unhandled case"); return {0.0f, 0.0f};
        }
    }


    void
    DrawableText::draw
    (
        SpriteBatch* renderer,
        const v2& p,
        const Rgb& base_hi_color
    ) const
    {
        draw(renderer, p, base_hi_color, base_hi_color);
    }


    void
    DrawableText::draw
    (
        SpriteBatch* renderer,
        const v2& p,
        const Rgb& base_color,
        const Rgb& hi_color
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
                e.with_inset(Lrtb{ -5.0f }).with_offset(p + off)
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


    Rect
    DrawableText::get_extents() const
    {
        compile();
        ASSERT(!is_dirty);
        return commands.get_extents();
    }
}
