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
    struct scalable_sprite;
    struct buffer2d;


    struct draw_data
    {
        draw_data();
        core::Angle rotation;
        core::Scale2f scale;
        core::Rgba tint;

        draw_data&
        set_rotation(const core::Angle& r);

        draw_data&
        set_scale(const core::Scale2f& s);

        draw_data&
        set_tint(const core::Rgba& t);
    };


    struct sprite_renderer
    {
        sprite_renderer(render::shader* shader);
        ~sprite_renderer();

        NONCOPYABLE(sprite_renderer);

        void
        draw_rect
        (
            const texture2d& texture,
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
            const texture2d& texture,
            const core::Rectf& position,
            const draw_data& data = draw_data{}
        );

        // bottom left
        void
        draw_ninepatch
        (
            const scalable_sprite& ninepatch,
            const core::Rectf& rect,
            const core::Rgba& tint
        );

    private:
        shader* sprite_shader;
        shader_uniform color_uniform;
        shader_uniform model_uniform;
        shader_uniform region_uniform;
        std::unique_ptr<buffer2d> single_quad_buffer;

        void
        init_render_data();
    };

}
