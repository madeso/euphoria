#pragma once

#include <memory>

#include "core/vec2.h"
#include "core/rgb.h"
#include "core/angle.h"
#include "core/rect.h"

#include "render/shaderuniform.h"
#include "render/shader.h"
#include "render/texture.h"

namespace euphoria::render
{
    struct ScalableSprite;
    struct Buffer2d;


    struct DrawData
    {
        core::angle rotation;
        core::Scale2f scale;
        core::rgba tint;

        DrawData();

        DrawData& set_rotation(const core::angle& r);
        DrawData& set_scale(const core::Scale2f& s);
        DrawData& set_tint(const core::rgba& t);
    };


    struct SpriteRenderer
    {
        SpriteRenderer(render::ShaderProgram* shader);
        ~SpriteRenderer();

        SpriteRenderer(const SpriteRenderer& other) = delete;
        void operator=(const SpriteRenderer&) = delete;
        SpriteRenderer(SpriteRenderer&& other) = delete;
        void operator=(SpriteRenderer&&) = delete;

        void
        draw_rect
        (
            const Texture2& texture,
            const core::Rectf& sprite_area,
            const core::Rectf& texture_region,
            const core::angle& rotation_angle,
            const core::Scale2f& rotation_anchor,
            const core::rgba& tint_color
        );

        // position is center
        void
        draw_sprite
        (
            const Texture2& texture,
            const core::Rectf& position,
            const DrawData& data = DrawData{}
        );

        // bottom left
        void
        draw_ninepatch
        (
            const ScalableSprite& ninepatch,
            const core::Rectf& rect,
            const core::rgba& tint
        );

    private:
        ShaderProgram* sprite_shader;
        ShaderUniform color_uniform;
        ShaderUniform model_uniform;
        ShaderUniform region_uniform;
        std::unique_ptr<Buffer2d> single_quad_buffer;

        void
        init_render_data();
    };

}
