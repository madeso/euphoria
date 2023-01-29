#include "runner/components.h"

#include <memory>

#include "core/ecs.h"
#include "core/vec2.h"
#include "core/cint.h"

#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

namespace euphoria::runner
{
    ComponentPosition2::ComponentPosition2()
        : pos(0, 0)
    {
    }

    ComponentPosition2::ComponentPosition2(const core::vec2f& p)
        : pos(p)
    {
    }

    Components::Components(core::ecs::Registry* reg)
        : position2(reg->register_component<ComponentPosition2>("position2"))
        , sprite(reg->register_component<ComponentSprite>("sprite"))
    {
    }

    core::Rectf
    get_sprite_rect(const core::vec2f& position, const render::Texture2& texture)
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
