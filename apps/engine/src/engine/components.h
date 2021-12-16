#pragma once

#include <memory>

#include "core/vec2.h"
#include "core/ecs.h"
#include "core/rect.h"

#include "render/texturecache.h"

namespace euphoria::engine
{
    struct ComponentPosition2 : public core::ecs::Component
    {
        ComponentPosition2();
        core::Vec2f pos;
    };

    struct ComponentSprite : public core::ecs::Component
    {
        COMPONENT_CONSTRUCTOR_DEFINITION(ComponentSprite)

        std::shared_ptr<render::Texture2> texture;
    };

    struct Components
    {
        Components(core::ecs::Registry* reg);

        core::ecs::ComponentId position2;
        core::ecs::ComponentId sprite;
    };

    core::Rectf
    get_sprite_rect
    (
        const core::Vec2f& position,
        const render::Texture2& texture
    );
}

TYPEID_SETUP_TYPE(euphoria::engine::ComponentPosition2);
TYPEID_SETUP_TYPE(euphoria::engine::ComponentSprite);

