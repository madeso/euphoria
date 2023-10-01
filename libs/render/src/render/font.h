#pragma once


#include <map>
#include <memory>

#include "core/loadedfont.h"
#include "base/vec2.h"
#include "base/vec3.h"
#include "base/rgb.h"
#include "base/rect.h"

#include "core/ui_text.h"

#include "render/texture.h"


namespace eu::io
{
    struct FileSystem;
    struct FilePath;
}


namespace eu::render
{
    struct SpriteRenderer;
    struct TextureCache;

    // todo(Gustav): separate rendering and the rest and move to core

    struct Glyph
    {
        Rectf sprite_rect; // relative to 0,0
        Rectf texture_rect; // image texture uvs
        int code_point; // the character or string id
        float advance;

        Glyph
        (
            const Rectf& sprite,
            const Rectf& texture,
            int ch,
            float ad
        );
    };

    using CharToGlyphMap = std::map<int, std::shared_ptr<Glyph>>;

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
        Rectf sprite_rect;
        Rectf texture_rect;
        bool hi;

        TextDrawCommand
        (
            const Texture2* texture,
            const Rectf& sprite_rect,
            const Rectf& texture_rect,
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
            const Rectf& sprite_rect,
            const Rectf& texture_rect,
            bool hi
        );

        void
        draw
        (
                SpriteRenderer* renderer,
                const vec2f& start_position,
                const Rgb& base_color,
                const Rgb& hi_color
        );

        [[nodiscard]] Rectf
        get_extents() const;
    };


    struct DrawableText
    {
    public:
        explicit DrawableText(DrawableFont* the_font);
        ~DrawableText();

        DrawableText(const DrawableText& other) = delete;
        void operator=(const DrawableText&) = delete;
        DrawableText(DrawableText&& other) = delete;
        void operator=(DrawableText&&) = delete;

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
            const vec2f& p,
            const Rgb& base_hi_color
        ) const;

        void
        draw
        (
            SpriteRenderer* renderer,
            const vec2f& p,
            const Rgb& base_color,
            const Rgb& hi_color
        ) const;

        Rectf
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


    struct UiTextCompileVisitor;


    struct DrawableFont
    {
        float line_height = 1;
        std::unique_ptr<Texture2> texture;
        std::shared_ptr<Texture2> background;
        CharToGlyphMap char_to_glyph;
        core::KerningMap kernings;
        std::map<std::string, int> private_use_aliases;

        DrawableFont
        (
            io::FileSystem* fs,
            TextureCache* cache,
            const io::FilePath& font_file
        );

        // todo(Gustav): expose background property and move this away from font
        void
        draw_background
        (
            SpriteRenderer* renderer,
            float alpha,
            const Rectf& where
        ) const;

        [[nodiscard]] ListOfTextDrawCommands compile_list
            (const core::UiText& text, float size) const;
    };
}
