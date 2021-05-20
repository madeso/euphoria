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
    struct sprite_renderer;

    struct sprite
    {
        explicit sprite(
                std::shared_ptr<texture2d> texture,
                const core::vec2f& position = core::vec2f::zero());

        [[nodiscard]] float
        get_height() const;

        [[nodiscard]] float
        get_width() const;

        void
        render(sprite_renderer* render) const;

        std::shared_ptr<texture2d> texture;
        core::vec2f position;
        core::angle rotation;
        core::scale2f scale = core::scale2f(1, 1);
        core::rgb color = core::rgb(1.0f);
        float alpha = 1.0f;
    };


    struct layer
    {
        explicit layer(sprite_renderer* render);

        void
        add(sprite* sprite);

        void
        remove(sprite* sprite);

        void
        render();

        using sprite_list = std::vector<sprite*>;
        using sprite_map = std::map<std::shared_ptr<texture2d>, sprite_list>;

        sprite_renderer* renderer;
        sprite_map texture_to_sprites;
    };
}
