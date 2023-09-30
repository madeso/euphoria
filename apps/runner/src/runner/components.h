#pragma once

#include <memory>

#include "base/vec2.h"
#include "core/rect.h"
#include "core/ecs.id.h"

#include "render/texturecache.h"

namespace eu::core::ecs
{
    struct Registry;
}


namespace eu::runner
{
    struct ComponentPosition2
    {
        ComponentPosition2();
        explicit ComponentPosition2(const vec2f& p);
        vec2f pos;
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
        const vec2f& position,
        const render::Texture2& texture
    );
}
