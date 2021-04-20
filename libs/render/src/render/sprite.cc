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
        return static_cast<float>(texture->get_height()) * scale.y;
    }


    float
    sprite::get_width() const
    {
        return static_cast<float>(texture->get_width()) * scale.x;
    }


    void
    sprite::render(SpriteRenderer* render) const
    {
        render->DrawSprite
        (
            *texture,
            core::Rectf::FromPositionAnchorWidthAndHeight
            (
                position,
                core::scale2f {0, 0},
                static_cast<float>(texture->get_width()),
                static_cast<float>(texture->get_height())
            ),
            DrawData{}
                .Rotation(rotation)
                .Scale(scale)
                .Tint(core::Rgba(color, alpha))
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
