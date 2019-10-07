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

    Components::Components(core::ecs::Registry* reg)
        : position2(reg->NewComponentType("Position2"))
        , sprite(reg->NewComponentType("Sprite"))
    {}

    COMPONENT_CONSTRUCTOR_IMPLEMENTATION(CSprite)

    core::Rectf
    GetSpriteRect(const core::vec2f& position, const render::Texture2d& texture)
    {
        return core::Rectf::FromPositionAnchorWidthAndHeight(
                position,
                core::scale2f {0.5f, 0.5f},
                texture.GetWidth(),
                texture.GetHeight());
    }
}  // namespace euphoria::engine
