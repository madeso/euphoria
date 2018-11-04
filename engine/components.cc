#include "engine/components.h"

#include <memory>

#include "core/componentsystem.h"
#include "core/vec2.h"

#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

CPosition2::CPosition2()
    : COMPONENT_CONSTRUCTOR_ARG(CPosition2) pos(0, 0)
{
}

Components::Components(Registry* reg)
    : position2(reg->NewComponentType("Position2"))
    , sprite(reg->NewComponentType("Sprite"))
{
}

COMPONENT_CONSTRUCTOR_IMPLEMENTATION(CSprite)

Rectf
GetSpriteRect(const point2f& position, const Texture2d& texture)
{
  return Rectf::FromPositionAnchorWidthAndHeight(
      position, scale2f{0.5f, 0.5f}, texture.GetWidth(), texture.GetHeight());
}
