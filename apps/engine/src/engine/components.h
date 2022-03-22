#pragma once

#include <memory>

#include "core/vec2.h"
#include "core/rect.h"
#include "core/ecs.id.h"

#include "render/texturecache.h"

namespace euphoria::core::ecs
{
    struct Registry;
}


namespace euphoria::engine
{
    struct ComponentPosition2
    {
        ComponentPosition2();
        explicit ComponentPosition2(const core::Vec2f& p);
        core::Vec2f pos;
    };

    struct ComponentSprite
    {
        std::shared_ptr<render::Texture2> texture;
    };

    struct Components
    {
        Components(core::ecs::Registry* reg);

        core::ecs::ComponentIndex position2;
        core::ecs::ComponentIndex sprite;
    };

    core::Rectf
    get_sprite_rect
    (
        const core::Vec2f& position,
        const render::Texture2& texture
    );
}
