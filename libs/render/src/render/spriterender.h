#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

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
        DrawData();
        core::Angle rotation;
        core::scale2f scale;
        core::Rgba tint;

        DrawData&
        Rotation(const core::Angle& r);

        DrawData&
        Scale(const core::scale2f& s);

        DrawData&
        Tint(const core::Rgba& t);
    };


    struct SpriteRenderer
    {
        SpriteRenderer(Shader* shader);
        ~SpriteRenderer();

        void
        DrawRect
        (
            const Texture2d& texture,
            const core::Rectf& sprite_area,
            const core::Rectf& texture_region,
            const core::Angle& rotation_angle,
            const core::scale2f& rotation_anchor,
            const core::Rgba& tint_color
        );

        // position is center
        void
        DrawSprite
        (
            const Texture2d& texture,
            const core::Rectf& position,
            const DrawData& data = DrawData {}
        );

        // bottom left
        void
        DrawNinepatch
        (
            const ScalableSprite& ninepatch,
            const core::Rectf& rect,
            const core::Rgba& tint
        );

    private:
        Shader* shader_;
        ShaderUniform color_;
        ShaderUniform model_;
        ShaderUniform texture_area_;
        std::unique_ptr<Buffer2d> buffer_;

        void
        InitRenderData();
    };

}

#endif
