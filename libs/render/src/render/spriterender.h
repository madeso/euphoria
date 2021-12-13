#pragma once

#include <memory>

#include "core/vec2.h"
#include "core/rgb.h"
#include "core/angle.h"
#include "core/rect.h"
#include "core/noncopyable.h"

#include "render/shaderuniform.h"
#include "render/shader.h"
#include "render/texture.h"

namespace euphoria::render
{
    struct ScalableSprite;
    struct Buffer2d;


    struct DrawData
    {
        DrawData();
        core::Angle rotation;
        core::Scale2f scale;
        core::Rgba tint;

        DrawData&
        set_rotation(const core::Angle& r);

        DrawData&
        set_scale(const core::Scale2f& s);

        DrawData&
        set_tint(const core::Rgba& t);
    };


    struct SpriteRenderer
    {
        SpriteRenderer(render::ShaderProgram* shader);
        ~SpriteRenderer();

        NONCOPYABLE(SpriteRenderer);

        void
        draw_rect
        (
            const Texture2& texture,
            const core::Rectf& sprite_area,
            const core::Rectf& texture_region,
            const core::Angle& rotation_angle,
            const core::Scale2f& rotation_anchor,
            const core::Rgba& tint_color
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
            const core::Rgba& tint
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
