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

    draw_data::draw_data()
        : rotation(0.0_rad)
        , scale(core::scale2f(1, 1))
        , tint(core::rgba(core::color::white))
    {
    }


    draw_data&
    draw_data::set_rotation(const core::angle& r)
    {
        rotation = r;
        return *this;
    }


    draw_data&
    draw_data::set_scale(const core::scale2f& s)
    {
        scale = s;
        return *this;
    }


    draw_data&
    draw_data::set_tint(const core::rgba& t)
    {
        tint = t;
        return *this;
    }


    //////////////////////////////////////////////////////////////////////////


    sprite_renderer::sprite_renderer(shader* shader)
        : sprite_shader(shader)
        , color_uniform(shader->get_uniform("color"))
        , model_uniform(shader->get_uniform("model"))
        , region_uniform(shader->get_uniform("region"))
    {
        sprite_shader = shader;
        init_render_data();
    }


    sprite_renderer::~sprite_renderer()
    {
        single_quad_buffer.reset();
    }


    void
    sprite_renderer::draw_rect
    (
        const texture2d& texture,
        const core::rectf& sprite_area,
        const core::rectf& texture_region,
        const core::angle& rotation_angle,
        const core::scale2f& rotation_anchor,
        const core::rgba& tint_color
    )
    {
        use(sprite_shader);

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
    sprite_renderer::draw_sprite
    (
        const texture2d& texture,
        const core::rectf& position,
        const draw_data& data
    )
    {
        draw_rect
        (
            texture,
            position,
            core::rectf::from_top_left_width_height(core::vec2f{0, 1}, 1, 1),
            data.rotation,
            core::scale2f{0.5f, 0.5f},
            data.tint
        );
    }


    void
    sprite_renderer::draw_ninepatch
    (
        const scalable_sprite& ninepatch,
        const core::rectf& rect,
        const core::rgba& tint
    )
    {
        ninepatch.render(this, rect, tint);
    }


    void
    sprite_renderer::init_render_data()
    {
        core::buffer_builder2d data;

        core::point a(0.0f, 0.0f, 0.0f, 0.0f);
        core::point b(1.0f, 0.0f, 1.0f, 0.0f);
        core::point c(0.0f, 1.0f, 0.0f, 1.0f);
        core::point d(1.0f, 1.0f, 1.0f, 1.0f);

        data.add_quad(a, b, c, d);

        single_quad_buffer = std::make_unique<buffer2d>(data);
    }
}

