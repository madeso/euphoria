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
    struct SpriteRenderer;
    struct TextureCache;

    // todo(Gustav): separate rendering and the rest and move to core

    struct Glyph
    {
        Glyph
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

    using char_to_glyph_map = std::map<int, std::shared_ptr<Glyph>>;

    struct DrawableFont;

    enum class Align
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


    struct TextDrawCommand
    {
        const Texture2* texture;
        core::Rectf sprite_rect;
        core::Rectf texture_rect;
        bool hi;

        TextDrawCommand
        (
            const Texture2* texture,
            const core::Rectf& sprite_rect,
            const core::Rectf& texture_rect,
            bool hi
        );
    };


    struct ListOfTextDrawCommands
    {
        std::vector<TextDrawCommand> commands;

        void
        add
        (
            const Texture2* texture,
            const core::Rectf& sprite_rect,
            const core::Rectf& texture_rect,
            bool hi
        );

        void
        draw
        (
                SpriteRenderer* renderer,
                const core::Vec2f& start_position,
                const core::Rgb& base_color,
                const core::Rgb& hi_color
        );

        [[nodiscard]] core::Rectf
        get_extents() const;
    };


    struct DrawableText
    {
    public:
        explicit DrawableText(DrawableFont* the_font);
        ~DrawableText();

        NONCOPYABLE(DrawableText);

        void
        set_text(const core::UiText& new_text);

        void
        set_background(bool new_use_background, float new_alpha = 0.5f);

        void
        set_alignment(Align new_alignment);

        void
        set_size(float new_size);

        void
        draw
        (
            SpriteRenderer* renderer,
            const core::Vec2f& p,
            const core::Rgb& base_hi_color
        ) const;

        void
        draw
        (
            SpriteRenderer* renderer,
            const core::Vec2f& p,
            const core::Rgb& base_color,
            const core::Rgb& hi_color
        ) const;

        core::Rectf
        get_extents() const;

        void
        compile() const;

    private:
        const DrawableFont* font;
        float size;
        core::UiText text;
        Align alignment;

        bool use_background;
        float background_alpha;

        // updated in Compile function
        mutable bool is_dirty;
        mutable ListOfTextDrawCommands commands;
    };


    struct ui_text_compile_visitor;


    struct DrawableFont
    {
    public:
        DrawableFont
        (
            core::vfs::FileSystem* fs,
            TextureCache* cache,
            const core::vfs::FilePath& font_file
        );

        // todo(Gustav): expose background property and move this away from font
        void
        draw_background
        (
            SpriteRenderer* renderer,
            float alpha,
            const core::Rectf& where
        ) const;

        [[nodiscard]] ListOfTextDrawCommands
        compile_list(const core::UiText& text, float size) const;

        float line_height=1;

        friend ui_text_compile_visitor;
        std::unique_ptr<Texture2> texture;
        std::shared_ptr<Texture2> background;
        char_to_glyph_map char_to_glyph;
        core::KerningMap kernings;
        std::map<std::string, int> private_use_aliases;
    };
}
