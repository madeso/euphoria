#include "engine/components.h"

#include <memory>

#include "core/ecs-systems.h"
#include "core/vec2.h"
#include "core/cint.h"

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

    components::components(core::ecs::Registry* reg)
        : position2(reg->register_new_component_type("position2"))
        , sprite(reg->register_new_component_type("sprite"))
    {
    }

    COMPONENT_CONSTRUCTOR_IMPLEMENTATION(CSprite)

    core::Rectf
    get_sprite_rect(const core::Vec2f& position, const render::texture2d& texture)
    {
        return core::Rectf::from_position_anchor_width_and_height
        (
            position,
            core::Scale2f {0.5f, 0.5f},
            core::c_int_to_float(texture.width),
            core::c_int_to_float(texture.height)
        );
    }
}
