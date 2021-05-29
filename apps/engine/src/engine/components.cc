#include "engine/components.h"

#include <memory>

#include "core/ecs-systems.h"
#include "core/vec2.h"

#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

namespace euphoria::engine
{
    component_position2::component_position2()
        : COMPONENT_CONSTRUCTOR_ARG(component_position2)
        pos(0, 0)
    {
    }

    components::components(core::ecs::registry* reg)
        : position2(reg->register_new_component_type("position2"))
        , sprite(reg->register_new_component_type("sprite"))
    {
    }

    COMPONENT_CONSTRUCTOR_IMPLEMENTATION(CSprite)

    core::rectf
    get_sprite_rect(const core::vec2f& position, const render::texture2d& texture)
    {
        return core::rectf::from_position_anchor_width_and_height
        (
            position,
            core::scale2f {0.5f, 0.5f},
            texture.width,
            texture.height
        );
    }
}
