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
    struct SpriteRenderer;
    struct TextureCache;

    // todo(Gustav): seperate rendering and the rest and move to core

    struct Glyph
    {
        Glyph
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

    using CharDataMap = std::map<int, std::shared_ptr<Glyph>>;

    struct Font;

    enum class Align
    {
        TOP_LEFT,
        TOP_CENTER,
        TOP_RIGHT,
        BASELINE_LEFT,
        BASELINE_CENTER,
        BASELINE_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_CENTER,
        BOTTOM_RIGHT
    };


    struct TextDrawCommand
    {
        const texture2d* texture;
        core::rectf sprite_rect;
        core::rectf texture_rect;
        bool hi;

        TextDrawCommand
        (
            const texture2d* texture,
            const core::rectf& sprite_rect,
            const core::rectf& texture_rect,
            bool hi
        );
    };


    struct TextDrawCommandList
    {
        std::vector<TextDrawCommand> commands;

        void
        Add
        (
            const texture2d* texture,
            const core::rectf& sprite_rect,
            const core::rectf& texture_rect,
            bool hi
        );

        void
        Draw
        (
            SpriteRenderer* renderer,
            const core::vec2f& start_position,
            const core::rgb& base_color,
            const core::rgb& hi_color
        );

        [[nodiscard]] core::rectf
        GetExtents() const;
    };


    struct Text
    {
    public:
        explicit Text(Font* font);
        ~Text();

        NONCOPYABLE(Text);

        void
        SetText(const core::ui_text& text);

        void
        SetBackground(bool use_background, float alpha = 0.5f);

        void
        SetAlignment(Align alignment);

        void
        SetSize(float new_size);

        void
        Draw
        (
            SpriteRenderer* renderer,
            const core::vec2f& p,
            const core::rgb& base_hi_color
        ) const;

        void
        Draw
        (
            SpriteRenderer* renderer,
            const core::vec2f& p,
            const core::rgb& base_color,
            const core::rgb& hi_color
        ) const;

        core::rectf
        GetExtents() const;

        void
        Compile() const;

    private:
        const Font* font_;
        float size_;
        core::ui_text text_;
        Align alignment_;

        bool use_background_;
        float background_alpha_;

        // updated in Compile function
        mutable bool dirty;
        mutable TextDrawCommandList commands;
    };


    struct UiTextCompileVisitor;


    struct Font
    {
    public:
        Font
        (
            core::vfs::file_system* fs,
            TextureCache* cache,
            const core::vfs::file_path& font_file
        );

        // todo(Gustav): expose background property and move this away from font
        void
        DrawBackground
        (
            SpriteRenderer* renderer,
            float alpha,
            const core::rectf& where
        ) const;

        [[nodiscard]] TextDrawCommandList
        CompileList(const core::ui_text& text, float size) const;

        float line_height=1;

        friend UiTextCompileVisitor;
        std::unique_ptr<texture2d> texture_;
        std::shared_ptr<texture2d> background;
        CharDataMap chars_;
        core::kerning_map kerning_;
        std::map<std::string, int> private_use_aliases;
    };
}
