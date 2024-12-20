#pragma once


#include <map>
#include <memory>

#include "base/vec2.h"
#include "base/vec3.h"
#include "base/rgb.h"

namespace eu::render
{
    struct Texture2;
    struct SpriteRenderer;

    struct Sprite
    {
        std::shared_ptr<Texture2> texture;
        vec2f position;
        Angle rotation;
        Scale2f scale = Scale2f(1, 1);
        Rgb color = Rgb(1.0f);
        float alpha = 1.0f;

        explicit Sprite
        (
            std::shared_ptr<Texture2> texture,
            const vec2f& position = zero2f
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
