#pragma once

#include <vector>
#include <map>
#include <memory>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/rgb.h"

namespace euphoria::render
{
    struct texture2d;
    struct SpriteRenderer;

    struct sprite
    {
        explicit sprite(
                std::shared_ptr<texture2d> texture,
                const core::vec2f& position = core::vec2f::Zero());

        [[nodiscard]] float
        get_height() const;

        [[nodiscard]] float
        get_width() const;

        void
        render(SpriteRenderer* render) const;

        std::shared_ptr<texture2d> texture;
        core::vec2f position;
        core::Angle rotation;
        core::scale2f scale = core::scale2f(1, 1);
        core::Rgb color = core::Rgb(1.0f);
        float alpha = 1.0f;
    };


    struct layer
    {
        explicit layer(SpriteRenderer* render);

        void
        add(sprite* sprite);

        void
        remove(sprite* sprite);

        void
        render();

        using sprite_list = std::vector<sprite*>;
        using sprite_map = std::map<std::shared_ptr<texture2d>, sprite_list>;

        SpriteRenderer* renderer;
        sprite_map texture_to_sprites;
    };
}
