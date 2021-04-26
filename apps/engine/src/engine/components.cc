#include "engine/components.h"

#include <memory>

#include "core/ecs-systems.h"
#include "core/vec2.h"

#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

namespace euphoria::engine
{
    CPosition2::CPosition2() : COMPONENT_CONSTRUCTOR_ARG(CPosition2) pos(0, 0)
    {}

    Components::Components(core::ecs::registry* reg)
        : position2(reg->register_new_component_type("Position2"))
        , sprite(reg->register_new_component_type("Sprite"))
    {}

    COMPONENT_CONSTRUCTOR_IMPLEMENTATION(CSprite)

    core::rectf
    GetSpriteRect(const core::vec2f& position, const render::texture2d& texture)
    {
        return core::rectf::from_position_anchor_width_and_height(
                position,
                core::scale2f {0.5f, 0.5f},
                texture.width,
                texture.height);
    }
}  // namespace euphoria::engine
