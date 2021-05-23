#pragma once

#include <memory>

#include "core/vec2.h"
#include "core/ecs.h"
#include "core/rect.h"

#include "render/texturecache.h"

namespace euphoria::engine
{
    struct component_position2 : public core::ecs::component
    {
        component_position2();
        core::vec2f pos;
    };

    struct component_sprite : public core::ecs::component
    {
        COMPONENT_CONSTRUCTOR_DEFINITION(component_sprite)

        std::shared_ptr<render::texture2d> texture;
    };

    struct components
    {
        components(core::ecs::registry* reg);

        core::ecs::component_id position2;
        core::ecs::component_id sprite;
    };

    core::rectf
    get_sprite_rect
    (
        const core::vec2f&       position,
        const render::texture2d& texture
    );
}

TYPEID_SETUP_TYPE(euphoria::engine::component_position2);
TYPEID_SETUP_TYPE(euphoria::engine::component_sprite);

