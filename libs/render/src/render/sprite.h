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
        std::shared_ptr<Texture2> texture;
        core::vec2f position;
        core::Angle rotation;
        core::Scale2f scale = core::Scale2f(1, 1);
        core::rgb color = core::rgb(1.0f);
        float alpha = 1.0f;

        explicit Sprite
        (
            std::shared_ptr<Texture2> texture,
            const core::vec2f& position = core::zero2f
        );

        void render(SpriteRenderer* render) const;

        [[nodiscard]] float get_height() const;
        [[nodiscard]] float get_width() const;
    };


    struct Layer
    {
        using SpriteList = std::vector<Sprite*>;
        using SpriteMap = std::map<std::shared_ptr<Texture2>, SpriteList>;

        SpriteRenderer* renderer;
        SpriteMap texture_to_sprites;

        explicit Layer(SpriteRenderer* render);

        void add(Sprite* sprite);
        void remove(Sprite* sprite);
        void render();
    };
}
