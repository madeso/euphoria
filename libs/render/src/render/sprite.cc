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
        const core::vec2f& position
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
    sprite::render(SpriteRenderer* render) const
    {
        render->DrawSprite
        (
            *texture,
            core::rectf::from_position_anchor_width_and_height
            (
                position,
                core::scale2f {0, 0},
                static_cast<float>(texture->width),
                static_cast<float>(texture->height)
            ),
            DrawData{}
                .Rotation(rotation)
                .Scale(scale)
                .Tint(core::rgba(color, alpha))
        );
    }


    layer::layer(SpriteRenderer* render)
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
