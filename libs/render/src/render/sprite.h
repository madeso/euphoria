#pragma once


#include <map>
#include <memory>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/rgb.h"

namespace euphoria::render
{
    struct Texture2;
    struct SpriteRenderer;

    struct Sprite
    {
        explicit Sprite
        (
            std::shared_ptr<Texture2> texture,
            const core::Vec2f& position = core::Vec2f::zero()
        );

        [[nodiscard]] float
        get_height() const;

        [[nodiscard]] float
        get_width() const;

        void
        render(SpriteRenderer* render) const;

        std::shared_ptr<Texture2> texture;
        core::Vec2f position;
        core::Angle rotation;
        core::Scale2f scale = core::Scale2f(1, 1);
        core::Rgb color = core::Rgb(1.0f);
        float alpha = 1.0f;
    };


    struct Layer
    {
        explicit Layer(SpriteRenderer* render);

        void
        add(Sprite* sprite);

        void
        remove(Sprite* sprite);

        void
        render();

        using SpriteList = std::vector<Sprite*>;
        using SpriteMap = std::map<std::shared_ptr<Texture2>, SpriteList>;

        SpriteRenderer* renderer;
        SpriteMap texture_to_sprites;
    };
}
