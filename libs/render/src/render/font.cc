#include "render/font.h"

#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <iostream>

#include "stb_rect_pack.h"

#include "core/log.h"
#include "core/proto.h"
#include "core/image_draw.h"
#include "core/ui_text.h"
#include "core/utf8.h"
#include "core/assert.h"
#include "core/noncopyable.h"
#include "core/image.h"
#include "core/vfs_path.h"
#include "core/stdutils.h"
#include "core/stringmerger.h"

#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

#include "gaf_font.h"
#include "gaf_pugixml_font.h"

using namespace euphoria::convert;


namespace euphoria::render
{
    glyph::glyph
    (
        const core::rectf& sprite,
        const core::rectf& texture,
        int ch,
        float ad
    )
        : sprite_rect(sprite)
        , texture_rect(texture)
        , code_point(ch)
        , advance(ad)
    {
    }


    core::loaded_font
    get_characters_from_single_image
    (
        core::vfs::file_system* fs,
        const font::SingleImage& img
    )
    {
        const auto image_file = core::vfs::file_path::from_script(img.file);

        if(image_file.has_value() == false)
        {
            LOG_WARN("Invalid path {0}", img.file);
            return core::loaded_font{};
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


    std::pair<core::rectf, core::rectf>
    construct_character_rects
    (
        const stbrp_rect& src_rect,
        const core::loaded_glyph& src_char,
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
        const stbrp_coord uv_top = uv_bottom + std::max(static_cast<stbrp_coord>(1), src_rect.h);

        // todo(Gustav): add ability to be a quad for tighter fit
        ASSERTX(vert_top > vert_bottom, vert_top, vert_bottom, src_char.code_point);
        ASSERTX(uv_top > uv_bottom, uv_top, uv_bottom, src_char.code_point);

        const auto iw = static_cast<float>(image_width);
        const auto ih = static_cast<float>(image_height);

        const auto sprite = core::rectf::from_left_right_top_bottom
        (
            static_cast<float>(vert_left),
            static_cast<float>(vert_right),
            static_cast<float>(vert_top),
            static_cast<float>(vert_bottom)
        );
        const auto texture = core::rectf::from_left_right_top_bottom
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


    drawable_font::drawable_font
    (
        core::vfs::file_system* fs,
        texture_cache* cache,
        const core::vfs::file_path& font_file
    )
    {
        // todo(Gustav): too long, break up
        const int texture_width = 512;
        const int texture_height = 512;

        background = cache->get_texture
        (
            core::vfs::file_path{"~/img-plain/white"}
        );

        core::loaded_font fontchars;

        const auto font_root = core::get_default_but_log_errors
        (
            core::read_xml_file_to_gaf_struct<font::Root>(fs, font_file, font::ReadXmlElementRoot)
        );

        // todo(Gustav): handle error better
        
        for(const auto& source: font_root.sources)
        {
            if(source.font)
            {
                const font::FontFile& font = *source.font;

                const auto p = core::vfs::file_path::from_script(font.file);
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

        char_to_glyph_map map;
        core::image image;
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

            std::shared_ptr<glyph> dest
            (
                new glyph
                (
                    sprite_and_texture_rects.first,
                    sprite_and_texture_rects.second,
                    src_char.code_point,
                    static_cast<float>(src_char.advance) / src_char.size
                )
            );
            map.insert(char_to_glyph_map::value_type(dest->code_point, dest));
        }

        // for debug
        // fs->WriteFile("charmap.png", image.Write(ImageWriteFormat::PNG));

        // load pixels into texture
        private_use_aliases = fontchars.private_use_aliases;
        kernings = fontchars.kerning;
        char_to_glyph = map;
        texture2d_load_data load_data;
        texture = std::make_unique<texture2d>();
        texture->load_from_image
        (
                image,
                core::alpha_load::keep, texture2d_load_data()
        );
        line_height = static_cast<float>(fontchars.line_height);
    }


    void
    drawable_font::draw_background
    (
        sprite_renderer* renderer,
        float alpha,
        const core::rectf& where
    ) const
    {
        renderer->draw_rect
        (
            *background,
            where,
            core::rectf::from_width_height(1, 1),
            0.0_rad,
            core::scale2f{0, 0},
            core::rgba{core::color::black, alpha}
        );
    }


    text_draw_command::text_draw_command
    (
        const texture2d* texture,
        const core::rectf& sprite_rect,
        const core::rectf& texture_rect,
        bool hi
    )
        : texture(texture)
        , sprite_rect(sprite_rect)
        , texture_rect(texture_rect)
        , hi(hi)
    {
    }


    void
    list_of_text_draw_commands::add
    (
        const texture2d* texture,
        const core::rectf& sprite_rect,
        const core::rectf& texture_rect,
        bool hi
    )
    {
        commands.emplace_back(texture, sprite_rect, texture_rect, hi);
    }


    void
    list_of_text_draw_commands::draw
    (
        sprite_renderer* renderer,
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
                core::scale2f{0.5f, 0.5f},
                core::rgba{tint}
            );
        }
    }


    struct ui_text_compile_visitor : public core::textparser::visitor
    {
        const drawable_font& font;
        float size;
        bool apply_highlight;
        core::vec2f position; // todo(Gustav): rename to offset
        int last_char_index = 0;

        // return value
        list_of_text_draw_commands* list;

        ui_text_compile_visitor
        (
            const drawable_font& f,
            float size,
            list_of_text_draw_commands* list
        )
            : font(f)
            , size(size)
            , apply_highlight(false)
            , position(0, 0)
            , list(list)
        {
        }


        void
        on_text(const std::string& text) override
        {
            core::utf8_to_codepoints
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
                std::shared_ptr<glyph> ch = it->second;

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


    list_of_text_draw_commands
    drawable_font::compile_list(const core::ui_text& text, float size) const
    {
        list_of_text_draw_commands list;

        ui_text_compile_visitor vis {*this, size, &list};
        text.accept(&vis);

        return list;
    }


    core::rectf
    list_of_text_draw_commands::get_extents() const
    {
        core::rectf ret;
        for(const auto& cmd: commands)
        {
            ret.include(cmd.sprite_rect);
        }
        return ret;
    }


    drawable_text::drawable_text(drawable_font* the_font)
        : font(the_font)
        , size(12.0f)
        , alignment(align::baseline_left)
        , use_background(false)
        , background_alpha(0.0f)
        , is_dirty(true)
    {
    }


    drawable_text::~drawable_text() = default;


    void
    drawable_text::set_text(const core::ui_text& new_text)
    {
        text = new_text;
        is_dirty = true;
    }


    void
    drawable_text::set_background(bool new_use_background, float new_alpha)
    {
        use_background = new_use_background;
        background_alpha = new_alpha;
    }


    void
    drawable_text::set_alignment(align new_alignment)
    {
        alignment = new_alignment;
    }


    void
    drawable_text::set_size(float new_size)
    {
        if(size != new_size)
        {
            size = new_size;
            is_dirty = true;
        }
    }


    core::vec2f
    get_offset(align alignment, const core::rectf& extent)
    {
        // todo(Gustav): test this more
        const auto middle = -(extent.left + extent.right) / 2;
        const auto right = -extent.right;
        const auto top = extent.top;
        const auto bottom = -extent.bottom;

        switch(alignment)
        {
        case align::top_left: return core::vec2f(0.0f, top);
        case align::top_center: return core::vec2f(middle, top);
        case align::top_right: return core::vec2f(right, top);
        case align::baseline_left: return core::vec2f(0.0f, 0.0f);
        case align::baseline_center: return core::vec2f(middle, 0.0f);
        case align::baseline_right: return core::vec2f(right, 0.0f);
        case align::bottom_left: return core::vec2f(0.0f, bottom);
        case align::bottom_center: return core::vec2f(middle, bottom);
        case align::bottom_right: return core::vec2f(right, bottom);
        default: DIE("Unhandled case"); return core::vec2f(0.0f, 0.0f);
        }
    }


    void
    drawable_text::draw
    (
        sprite_renderer* renderer,
        const core::vec2f& p,
        const core::rgb& base_hi_color
    ) const
    {
        draw(renderer, p, base_hi_color, base_hi_color);
    }


    void
    drawable_text::draw
    (
        sprite_renderer* renderer,
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
    drawable_text::compile() const
    {
        if(is_dirty)
        {
            is_dirty = false;
            commands = font->compile_list(text, size);
        }
    }


    core::rectf
    drawable_text::get_extents() const
    {
        compile();
        ASSERT(!is_dirty);
        return commands.get_extents();
    }
}
