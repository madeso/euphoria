#include "render/sprite.h"

#include <algorithm>

#include "core/vec4.h"

#include "render/spriterender.h"

using namespace euphoria::convert;

namespace euphoria::render
{
    Sprite::Sprite
    (
        std::shared_ptr<Texture2> t,
        const core::Vec2f& p
    )
        : texture(t)
        , position(p)
        , rotation(0.0_rad)
    {
    }


    float
    Sprite::get_height() const
    {
        return static_cast<float>(texture->height) * scale.y;
    }


    float
    Sprite::get_width() const
    {
        return static_cast<float>(texture->width) * scale.x;
    }


    void
    Sprite::render(SpriteRenderer* render) const
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
            DrawData{}
                .set_rotation(rotation)
                .set_scale(scale)
                .set_tint(core::Rgba(color, alpha))
        );
    }


    Layer::Layer(SpriteRenderer* render)
        : renderer(render)
    {
    }


    void
    Layer::add(Sprite* sprite)
    {
        texture_to_sprites[sprite->texture].push_back(sprite);
    }


    void
    Layer::remove(Sprite* sprite)
    {
        SpriteList& sprites = texture_to_sprites[sprite->texture];
        sprites.erase
        (
            std::remove(sprites.begin(), sprites.end(), sprite),
            sprites.end()
        );
    }


    void
    Layer::render()
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
