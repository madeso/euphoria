#pragma once


#include <map>
#include <memory>

#include "eu/base/vec2.h"
#include "eu/base/vec3.h"
#include "eu/base/colors.h"
#include "eu/base/rect.h"

#include "eu/core/loadedfont.h"
// #include "eu/core/ui_text.h"

#include "eu/render/texture.h"


namespace eu::render
{
    struct SpriteBatch;

    // todo(Gustav): separate rendering and the rest and move to core

    struct Glyph
    {
        Rect sprite_rect; // relative to 0,0
        Rect texture_rect; // image texture uvs
        int code_point; // the character or string id
        float advance;

        Glyph
        (
            const Rect& sprite,
            const Rect& texture,
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
        const Texture2d* texture;
        Rect sprite_rect;
        Rect texture_rect;
        bool hi;

        TextDrawCommand
        (
            const Texture2d* texture,
            const Rect& sprite_rect,
            const Rect& texture_rect,
            bool hi
        );
    };


    struct ListOfTextDrawCommands
    {
        std::vector<TextDrawCommand> commands;

        void
        add
        (
            const Texture2d* texture,
            const Rect& sprite_rect,
            const Rect& texture_rect,
            bool hi
        );

        void
        draw
        (
                SpriteBatch* renderer,
                const v2& start_position,
                const Rgb& base_color,
                const Rgb& hi_color
        );

        [[nodiscard]] Rect
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
        set_text(const std::string& new_text);

        void
        set_background(bool new_use_background, float new_alpha = 0.5f);

        void
        set_alignment(Align new_alignment);

        void
        set_size(float new_size);

        void
        draw
        (
            SpriteBatch* renderer,
            const v2& p,
            const Rgb& base_hi_color
        ) const;

        void
        draw
        (
            SpriteBatch* renderer,
            const v2& p,
            const Rgb& base_color,
            const Rgb& hi_color
        ) const;

        Rect
        get_extents() const;

        void
        compile() const;

    private:
        const DrawableFont* font;
        float size;
        std::string text;
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
        float ascent = 0;
        float descent = 0;
        float line_gap = 0;
        constexpr float get_line_height() const
        {
            return ascent - descent + line_gap;
        }

        std::unique_ptr<Texture2d> texture;
        CharToGlyphMap char_to_glyph;
        core::KerningMap kernings;
        std::map<std::string, int> private_use_aliases;

        explicit DrawableFont
        (
            const MemoryChunk& font_file
        );

        // todo(Gustav): expose background property and move this away from font
        void
        draw_background
        (
            SpriteBatch* renderer,
            float alpha,
            const Rect& where
        ) const;

        [[nodiscard]] ListOfTextDrawCommands compile_list
            (const std::string& text, float size) const;
    };
}
