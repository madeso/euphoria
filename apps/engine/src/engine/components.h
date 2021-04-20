#ifndef EUPHORIA_COMPONENTS_H
#define EUPHORIA_COMPONENTS_H

#include <memory>

#include "core/vec2.h"
#include "core/ecs.h"
#include "core/rect.h"

#include "render/texturecache.h"

namespace euphoria::engine
{
    struct CPosition2 : public core::ecs::Component
    {
        CPosition2();
        core::vec2f pos;
    };

    struct CSprite : public core::ecs::Component
    {
        COMPONENT_CONSTRUCTOR_DEFINITION(CSprite)

        std::shared_ptr<render::texture2d> texture;
    };

    struct Components
    {
        Components(core::ecs::Registry* reg);

        core::ecs::ComponentId position2;
        core::ecs::ComponentId sprite;
    };

    core::Rectf
    GetSpriteRect(
            const core::vec2f&       position,
            const render::texture2d& texture);
}  // namespace euphoria::engine

TYPEID_SETUP_TYPE(euphoria::engine::CPosition2);
TYPEID_SETUP_TYPE(euphoria::engine::CSprite);

#endif  // EUPHORIA_COMPONENTS_H
