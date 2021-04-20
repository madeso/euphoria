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

    // todo(Gustav): seperate rendering and the rest and move to core

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
        core::Rectf sprite_rect;
        core::Rectf texture_rect;
        bool hi;

        TextDrawCommand
        (
            const texture2d* texture,
            const core::Rectf& sprite_rect,
            const core::Rectf& texture_rect,
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
            const core::Rectf& sprite_rect,
            const core::Rectf& texture_rect,
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

        [[nodiscard]] core::Rectf
        GetExtents() const;
    };


    struct Text
    {
    public:
        explicit Text(Font* font);
        ~Text();

        NONCOPYABLE(Text);

        void
        SetText(const core::UiText& text);

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

        core::Rectf
        GetExtents() const;

        void
        Compile() const;

    private:
        const Font* font_;
        float size_;
        core::UiText text_;
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
            core::vfs::FileSystem* fs,
            TextureCache* cache,
            const core::vfs::FilePath& font_file
        );

        // todo(Gustav): expose background property and move this away from font
        void
        DrawBackground
        (
            SpriteRenderer* renderer,
            float alpha,
            const core::Rectf& where
        ) const;

        [[nodiscard]] TextDrawCommandList
        CompileList(const core::UiText& text, float size) const;

        float line_height=1;

        friend UiTextCompileVisitor;
        std::unique_ptr<texture2d> texture_;
        std::shared_ptr<texture2d> background;
        CharDataMap chars_;
        core::KerningMap kerning_;
        std::map<std::string, int> private_use_aliases;
    };
}
