#include "render/spriterender.h"

#include "core/bufferbuilder2d.h"

#include "render/buffer2d.h"
#include "render/scalablesprite.h"
#include "render/shaderattribute2d.h"
#include "render/gl.h"


using namespace euphoria::convert;


namespace euphoria::render
{
    //////////////////////////////////////////////////////////////////////////

    DrawData::DrawData()
        : rotation(0.0_rad)
        , scale(core::scale2f(1, 1))
        , tint(core::Rgba(core::Color::White))
    {
    }


    DrawData&
    DrawData::Rotation(const core::Angle& r)
    {
        rotation = r;
        return *this;
    }


    DrawData&
    DrawData::Scale(const core::scale2f& s)
    {
        scale = s;
        return *this;
    }


    DrawData&
    DrawData::Tint(const core::Rgba& t)
    {
        tint = t;
        return *this;
    }


    //////////////////////////////////////////////////////////////////////////


    SpriteRenderer::SpriteRenderer(Shader* shader)
        : shader_(shader)
        , color_(shader->GetUniform("color"))
        , model_(shader->GetUniform("model"))
        , texture_area_(shader->GetUniform("region"))
    {
        shader_ = shader;
        InitRenderData();
    }


    SpriteRenderer::~SpriteRenderer()
    {
        buffer_.reset();
    }


    void
    SpriteRenderer::DrawRect
    (
        const texture2d& texture,
        const core::Rectf& sprite_area,
        const core::Rectf& texture_region,
        const core::Angle& rotation_angle,
        const core::scale2f& rotation_anchor,
        const core::Rgba& tint_color
    )
    {
        Use(shader_);

        core::vec3f rotation_anchor_displacement
        {
            -rotation_anchor.x * sprite_area.GetWidth(),
            (rotation_anchor.y - 1) * sprite_area.GetHeight(),
            0.0f
        };

        const core::mat4f model = core::mat4f::Identity()
            .Translate(core::vec3f(sprite_area.BottomLeft(), 0.0f))
            .Translate(-rotation_anchor_displacement)
            // rotate around center
            .Rotate
            (
                core::AxisAngle::RightHandAround
                (
                    core::unit3f::ZAxis(), 
                    rotation_angle
                )
            )
            .Translate(rotation_anchor_displacement)
            .Scale
            (
                core::scale3f
                {
                    sprite_area.GetWidth(),
                    sprite_area.GetHeight(),
                    1.0f
                }
            );

        shader_->SetUniform(model_, model);
        shader_->SetUniform(color_, tint_color);
        shader_->SetUniform(texture_area_, texture_region);

        glActiveTexture(GL_TEXTURE0);
        Use(&texture);
        buffer_->Draw();
    }


    void
    SpriteRenderer::DrawSprite
    (
        const texture2d& texture,
        const core::Rectf& position,
        const DrawData& data
    )
    {
        DrawRect
        (
            texture,
            position,
            core::Rectf::FromTopLeftWidthHeight(core::vec2f{0, 1}, 1, 1),
            data.rotation,
            core::scale2f {0.5f, 0.5f},
            data.tint
        );
    }


    void
    SpriteRenderer::DrawNinepatch
    (
        const ScalableSprite& ninepatch,
        const core::Rectf& rect,
        const core::Rgba& tint
    )
    {
        ninepatch.Render(this, rect, tint);
    }


    void
    SpriteRenderer::InitRenderData()
    {
        core::BufferBuilder2d data;

        core::Point b(1.0f, 0.0f, 1.0f, 0.0f);
        core::Point a(0.0f, 1.0f, 0.0f, 1.0f);
        core::Point c(0.0f, 0.0f, 0.0f, 0.0f);
        core::Point d(1.0f, 1.0f, 1.0f, 1.0f);

        data.AddQuad(c, b, a, d);

        buffer_ = std::make_unique<Buffer2d>(data);
    }
}

