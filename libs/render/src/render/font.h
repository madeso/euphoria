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
    struct file_system;
    struct file_path;
}


namespace euphoria::render
{
    struct sprite_renderer;
    struct texture_cache;

    // todo(Gustav): seperate rendering and the rest and move to core

    struct glyph
    {
        glyph
        (
            const core::rectf& sprite,
            const core::rectf& texture,
            int ch,
            float ad
        );

        core::rectf sprite_rect; // relative to 0,0
        core::rectf texture_rect; // image texture uvs
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
        core::rectf sprite_rect;
        core::rectf texture_rect;
        bool hi;

        text_draw_command
        (
            const texture2d* texture,
            const core::rectf& sprite_rect,
            const core::rectf& texture_rect,
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
            const core::rectf& sprite_rect,
            const core::rectf& texture_rect,
            bool hi
        );

        void
        draw
        (
                sprite_renderer* renderer,
                const core::vec2f& start_position,
                const core::rgb& base_color,
                const core::rgb& hi_color
        );

        [[nodiscard]] core::rectf
        get_extents() const;
    };


    struct drawable_text
    {
    public:
        explicit drawable_text(drawable_font* font);
        ~drawable_text();

        NONCOPYABLE(drawable_text);

        void
        set_text(const core::ui_text& text);

        void
        set_background(bool use_background, float alpha = 0.5f);

        void
        set_alignment(align alignment);

        void
        set_size(float new_size);

        void
        draw
        (
                sprite_renderer* renderer,
                const core::vec2f& p,
                const core::rgb& base_hi_color
        ) const;

        void
        draw
        (
                sprite_renderer* renderer,
                const core::vec2f& p,
                const core::rgb& base_color,
                const core::rgb& hi_color
        ) const;

        core::rectf
        get_extents() const;

        void
        compile() const;

    private:
        const drawable_font* font_;
        float size_;
        core::ui_text text_;
        align alignment_;

        bool use_background_;
        float background_alpha_;

        // updated in Compile function
        mutable bool dirty;
        mutable list_of_text_draw_commands commands;
    };


    struct ui_text_compile_visitor;


    struct drawable_font
    {
    public:
        drawable_font
        (
                core::vfs::file_system* fs,
                texture_cache* cache,
                const core::vfs::file_path& font_file
        );

        // todo(Gustav): expose background property and move this away from font
        void
        draw_background
        (
                sprite_renderer* renderer,
                float alpha,
                const core::rectf& where
        ) const;

        [[nodiscard]] list_of_text_draw_commands
        compile_list(const core::ui_text& text, float size) const;

        float line_height=1;

        friend ui_text_compile_visitor;
        std::unique_ptr<texture2d> texture_;
        std::shared_ptr<texture2d> background;
        char_to_glyph_map chars_;
        core::kerning_map kerning_;
        std::map<std::string, int> private_use_aliases;
    };
}
