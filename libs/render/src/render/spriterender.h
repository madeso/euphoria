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
        core::angle rotation;
        core::scale2f scale;
        core::rgba tint;

        draw_data&
        set_rotation(const core::angle& r);

        draw_data&
        set_scale(const core::scale2f& s);

        draw_data&
        set_tint(const core::rgba& t);
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
            const core::rectf& sprite_area,
            const core::rectf& texture_region,
            const core::angle& rotation_angle,
            const core::scale2f& rotation_anchor,
            const core::rgba& tint_color
        );

        // position is center
        void
        draw_sprite
        (
            const texture2d& texture,
            const core::rectf& position,
            const draw_data& data = draw_data{}
        );

        // bottom left
        void
        draw_ninepatch
        (
            const scalable_sprite& ninepatch,
            const core::rectf& rect,
            const core::rgba& tint
        );

    private:
        shader* shader_;
        shader_uniform color_;
        shader_uniform model_;
        shader_uniform texture_area_;
        std::unique_ptr<buffer2d> buffer_;

        void
        init_render_data();
    };

}
