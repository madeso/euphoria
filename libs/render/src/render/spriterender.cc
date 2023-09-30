#include "render/spriterender.h"

#include "core/bufferbuilder2d.h"

#include "render/buffer2d.h"
#include "render/scalablesprite.h"
#include "render/shaderattribute2d.h"
#include "render/gl.h"


using namespace eu::convert;


namespace eu::render
{
    //////////////////////////////////////////////////////////////////////////

    DrawData::DrawData()
        : rotation(0.0_rad)
        , scale(Scale2f(1, 1))
        , tint(core::Rgba(core::NamedColor::white))
    {
    }


    DrawData&
    DrawData::set_rotation(const Angle& r)
    {
        rotation = r;
        return *this;
    }


    DrawData&
    DrawData::set_scale(const Scale2f& s)
    {
        scale = s;
        return *this;
    }


    DrawData&
    DrawData::set_tint(const core::Rgba& t)
    {
        tint = t;
        return *this;
    }


    //////////////////////////////////////////////////////////////////////////


    SpriteRenderer::SpriteRenderer(ShaderProgram* shader)
        : sprite_shader(shader)
        , color_uniform(shader->get_uniform("color"))
        , model_uniform(shader->get_uniform("model"))
        , region_uniform(shader->get_uniform("region"))
    {
        init_render_data();
    }


    SpriteRenderer::~SpriteRenderer()
    {
        single_quad_buffer.reset();
    }


    void
    SpriteRenderer::draw_rect
    (
        const Texture2& texture,
        const core::Rectf& sprite_area,
        const core::Rectf& texture_region,
        const Angle& rotation_angle,
        const Scale2f& rotation_anchor,
        const core::Rgba& tint_color
    )
    {
        use(sprite_shader);

        vec3f rotation_anchor_displacement
        {
            -rotation_anchor.x * sprite_area.get_width(),
            (rotation_anchor.y - 1) * sprite_area.get_height(),
            0.0f
        };

        const mat4f model = m4_identity
            .get_translated(vec3f(sprite_area.get_bottom_left(), 0.0f))
            .get_translated(-rotation_anchor_displacement)
            // rotate around center
            .get_rotated
            (
                AxisAngle::from_right_hand_around
                (
                    common::z_axis, 
                    rotation_angle
                )
            )
            .get_translated(rotation_anchor_displacement)
            .get_scaled
            (
                Scale3f
                {
                    sprite_area.get_width(),
                    sprite_area.get_height(),
                    1.0f
                }
            )
            ;

        sprite_shader->set_uniform(model_uniform, model);
        sprite_shader->set_uniform(color_uniform, tint_color);
        sprite_shader->set_uniform(region_uniform, texture_region);

        glActiveTexture(GL_TEXTURE0);
        use(&texture);
        single_quad_buffer->draw();
    }


    void
    SpriteRenderer::draw_sprite
    (
        const Texture2& texture,
        const core::Rectf& position,
        const DrawData& data
    )
    {
        draw_rect
        (
            texture,
            position,
            core::Rectf::from_top_left_width_height(vec2f{0, 1}, 1, 1),
            data.rotation,
            Scale2f{0.5f, 0.5f},
            data.tint
        );
    }


    void
    SpriteRenderer::draw_ninepatch
    (
        const ScalableSprite& ninepatch,
        const core::Rectf& rect,
        const core::Rgba& tint
    )
    {
        ninepatch.render(this, rect, tint);
    }


    void
    SpriteRenderer::init_render_data()
    {
        core::BufferBuilder2 data;

        core::BufferPoint2 a(0.0f, 0.0f, 0.0f, 0.0f);
        core::BufferPoint2 b(1.0f, 0.0f, 1.0f, 0.0f);
        core::BufferPoint2 c(0.0f, 1.0f, 0.0f, 1.0f);
        core::BufferPoint2 d(1.0f, 1.0f, 1.0f, 1.0f);

        data.add_quad(a, b, c, d);

        single_quad_buffer = std::make_unique<Buffer2d>(data);
    }
}

