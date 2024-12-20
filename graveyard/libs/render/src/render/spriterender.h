#pragma once

#include <memory>

#include "base/vec2.h"
#include "base/rgb.h"
#include "base/angle.h"
#include "base/rect.h"

#include "render/shaderuniform.h"
#include "render/shader.h"
#include "render/texture.h"

namespace eu::render
{
    struct ScalableSprite;
    struct Buffer2d;


    struct DrawData
    {
        Angle rotation;
        Scale2f scale;
        Rgba tint;

        DrawData();

        DrawData& set_rotation(const Angle& r);
        DrawData& set_scale(const Scale2f& s);
        DrawData& set_tint(const Rgba& t);
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
            const Rectf& sprite_area,
            const Rectf& texture_region,
            const Angle& rotation_angle,
            const Scale2f& rotation_anchor,
            const Rgba& tint_color
        );

        // position is center
        void
        draw_sprite
        (
            const Texture2& texture,
            const Rectf& position,
            const DrawData& data = DrawData{}
        );

        // bottom left
        void
        draw_ninepatch
        (
            const ScalableSprite& ninepatch,
            const Rectf& rect,
            const Rgba& tint
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
