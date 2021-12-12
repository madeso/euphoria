#include "render/sprite.h"

#include <algorithm>

#include "core/vec4.h"

#include "render/spriterender.h"

using namespace euphoria::convert;

namespace euphoria::render
{
    sprite::sprite
    (
        std::shared_ptr<texture2d> texture,
        const core::Vec2f& position
    )
        : texture(texture)
        , position(position)
        , rotation(0.0_rad)
    {
    }


    float
    sprite::get_height() const
    {
        return static_cast<float>(texture->height) * scale.y;
    }


    float
    sprite::get_width() const
    {
        return static_cast<float>(texture->width) * scale.x;
    }


    void
    sprite::render(sprite_renderer* render) const
    {
        render->draw_sprite
        (
            *texture,
            core::Rectf::from_position_anchor_width_and_height
            (
                position,
                core::Scale2f{0, 0},
                static_cast<float>(texture->width),
                static_cast<float>(texture->height)
            ),
            draw_data{}
                .set_rotation(rotation)
                .set_scale(scale)
                .set_tint(core::Rgba(color, alpha))
        );
    }


    layer::layer(sprite_renderer* render)
        : renderer(render)
    {
    }


    void
    layer::add(sprite* sprite)
    {
        texture_to_sprites[sprite->texture].push_back(sprite);
    }


    void
    layer::remove(sprite* sprite)
    {
        sprite_list& sprites = texture_to_sprites[sprite->texture];
        sprites.erase
        (
            std::remove(sprites.begin(), sprites.end(), sprite),
            sprites.end()
        );
    }


    void
    layer::render()
    {
        for(auto& list: texture_to_sprites)
        {
            for(auto& sp: list.second)
            {
                sp->render(renderer);
            }
        }
    }

}
