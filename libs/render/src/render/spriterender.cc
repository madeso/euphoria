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
        , tint(core::rgba(core::color::white))
    {
    }


    DrawData&
    DrawData::Rotation(const core::angle& r)
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
    DrawData::Tint(const core::rgba& t)
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
        const core::rectf& sprite_area,
        const core::rectf& texture_region,
        const core::angle& rotation_angle,
        const core::scale2f& rotation_anchor,
        const core::rgba& tint_color
    )
    {
        Use(shader_);

        core::vec3f rotation_anchor_displacement
        {
            -rotation_anchor.x * sprite_area.get_width(),
            (rotation_anchor.y - 1) * sprite_area.get_height(),
            0.0f
        };

        const core::mat4f model = core::mat4f::identity()
            .translate(core::vec3f(sprite_area.get_bottom_left(), 0.0f))
            .translate(-rotation_anchor_displacement)
            // rotate around center
            .rotate
            (
                core::axis_angle::right_hand_around
                (
                    core::unit3f::z_axis(), 
                    rotation_angle
                )
            )
            .translate(rotation_anchor_displacement)
            .scale
            (
                core::scale3f
                {
                    sprite_area.get_width(),
                    sprite_area.get_height(),
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
        const core::rectf& position,
        const DrawData& data
    )
    {
        DrawRect
        (
            texture,
            position,
            core::rectf::from_top_left_width_height(core::vec2f{0, 1}, 1, 1),
            data.rotation,
            core::scale2f {0.5f, 0.5f},
            data.tint
        );
    }


    void
    SpriteRenderer::DrawNinepatch
    (
        const ScalableSprite& ninepatch,
        const core::rectf& rect,
        const core::rgba& tint
    )
    {
        ninepatch.Render(this, rect, tint);
    }


    void
    SpriteRenderer::InitRenderData()
    {
        core::buffer_builder2d data;

        core::point b(1.0f, 0.0f, 1.0f, 0.0f);
        core::point a(0.0f, 1.0f, 0.0f, 1.0f);
        core::point c(0.0f, 0.0f, 0.0f, 0.0f);
        core::point d(1.0f, 1.0f, 1.0f, 1.0f);

        data.add_quad(c, b, a, d);

        buffer_ = std::make_unique<Buffer2d>(data);
    }
}

