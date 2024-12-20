#include "runner/components.h"

#include <memory>

#include "core/ecs.h"
#include "base/vec2.h"
#include "base/cint.h"

#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

namespace eu::runner
{
    ComponentPosition2::ComponentPosition2()
        : pos(0, 0)
    {
    }

    ComponentPosition2::ComponentPosition2(const vec2f& p)
        : pos(p)
    {
    }

    Components::Components(core::ecs::Registry* reg)
        : position2(reg->register_component<ComponentPosition2>("position2"))
        , sprite(reg->register_component<ComponentSprite>("sprite"))
    {
    }

    Rectf
    get_sprite_rect(const vec2f& position, const render::Texture2& texture)
    {
        return Rectf::from_position_anchor_width_and_height
        (
            position,
            Scale2f{0.5f, 0.5f},
            c_int_to_float(texture.width),
            c_int_to_float(texture.height)
        );
    }
}
