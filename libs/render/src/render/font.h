#pragma once

#include <string>
#include <map>
#include <memory>

#include "core/loadedfont.h"
#include "core/vec2.h"
#include "core/vec3.h"
#include "core/rgb.h"
#include "core/rect.h"

#include "core/ui_text.h"

#include "render/texture.h"


namespace euphoria::core::vfs
{
    struct FileSystem;
    struct FilePath;
}


namespace euphoria::render
{
    struct sprite_renderer;
    struct texture_cache;

    // todo(Gustav): separate rendering and the rest and move to core

    struct glyph
    {
        glyph
        (
            const core::Rectf& sprite,
            const core::Rectf& texture,
            int ch,
            float ad
        );

        core::Rectf sprite_rect; // relative to 0,0
        core::Rectf texture_rect; // image texture uvs
        int code_point; // the character or string id
        float advance;
    };

    using char_to_glyph_map = std::map<int, std::shared_ptr<glyph>>;

    struct drawable_font;

    enum class align
    {
        top_left,
        top_center,
        top_right,
        baseline_left,
        baseline_center,
        baseline_right,
        bottom_left,
        bottom_center,
        bottom_right
    };


    struct text_draw_command
    {
        const texture2d* texture;
        core::Rectf sprite_rect;
        core::Rectf texture_rect;
        bool hi;

        text_draw_command
        (
            const texture2d* texture,
            const core::Rectf& sprite_rect,
            const core::Rectf& texture_rect,
            bool hi
        );
    };


    struct list_of_text_draw_commands
    {
        std::vector<text_draw_command> commands;

        void
        add
        (
            const texture2d* texture,
            const core::Rectf& sprite_rect,
            const core::Rectf& texture_rect,
            bool hi
        );

        void
        draw
        (
                sprite_renderer* renderer,
                const core::Vec2f& start_position,
                const core::Rgb& base_color,
                const core::Rgb& hi_color
        );

        [[nodiscard]] core::Rectf
        get_extents() const;
    };


    struct drawable_text
    {
    public:
        explicit drawable_text(drawable_font* the_font);
        ~drawable_text();

        NONCOPYABLE(drawable_text);

        void
        set_text(const core::UiText& new_text);

        void
        set_background(bool new_use_background, float new_alpha = 0.5f);

        void
        set_alignment(align new_alignment);

        void
        set_size(float new_size);

        void
        draw
        (
            sprite_renderer* renderer,
            const core::Vec2f& p,
            const core::Rgb& base_hi_color
        ) const;

        void
        draw
        (
            sprite_renderer* renderer,
            const core::Vec2f& p,
            const core::Rgb& base_color,
            const core::Rgb& hi_color
        ) const;

        core::Rectf
        get_extents() const;

        void
        compile() const;

    private:
        const drawable_font* font;
        float size;
        core::UiText text;
        align alignment;

        bool use_background;
        float background_alpha;

        // updated in Compile function
        mutable bool is_dirty;
        mutable list_of_text_draw_commands commands;
    };


    struct ui_text_compile_visitor;


    struct drawable_font
    {
    public:
        drawable_font
        (
            core::vfs::FileSystem* fs,
            texture_cache* cache,
            const core::vfs::FilePath& font_file
        );

        // todo(Gustav): expose background property and move this away from font
        void
        draw_background
        (
            sprite_renderer* renderer,
            float alpha,
            const core::Rectf& where
        ) const;

        [[nodiscard]] list_of_text_draw_commands
        compile_list(const core::UiText& text, float size) const;

        float line_height=1;

        friend ui_text_compile_visitor;
        std::unique_ptr<texture2d> texture;
        std::shared_ptr<texture2d> background;
        char_to_glyph_map char_to_glyph;
        core::KerningMap kernings;
        std::map<std::string, int> private_use_aliases;
    };
}
