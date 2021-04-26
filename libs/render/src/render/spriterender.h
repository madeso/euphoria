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
        core::angle rotation;
        core::scale2f scale;
        core::rgba tint;

        DrawData&
        Rotation(const core::angle& r);

        DrawData&
        Scale(const core::scale2f& s);

        DrawData&
        Tint(const core::rgba& t);
    };


    struct SpriteRenderer
    {
        SpriteRenderer(Shader* shader);
        ~SpriteRenderer();

        NONCOPYABLE(SpriteRenderer);

        void
        DrawRect
        (
            const texture2d& texture,
            const core::rectf& sprite_area,
            const core::rectf& texture_region,
            const core::angle& rotation_angle,
            const core::scale2f& rotation_anchor,
            const core::rgba& tint_color
        );

        // position is center
        void
        DrawSprite
        (
            const texture2d& texture,
            const core::rectf& position,
            const DrawData& data = DrawData {}
        );

        // bottom left
        void
        DrawNinepatch
        (
            const ScalableSprite& ninepatch,
            const core::rectf& rect,
            const core::rgba& tint
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
