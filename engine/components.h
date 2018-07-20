#ifndef EUPHORIA_COMPONENTS_H
#define EUPHORIA_COMPONENTS_H

#include <memory>

#include "core/vec2.h"
#include "core/entity.h"
#include "core/rect.h"

#include "render/texturecache.h"

struct CPosition2 : public Component
{
  CPosition2();
  vec2f pos;
};

struct CSprite : public Component
{
  COMPONENT_CONSTRUCTOR_DEFINITION(CSprite)

  std::shared_ptr<Texture2d> texture;
};

struct Components
{
  Components(Registry* reg);

  ComponentId position2;
  ComponentId sprite;
};

Rectf
GetSpriteRect(const vec2f& position, const Texture2d& texture);

#endif  // EUPHORIA_COMPONENTS_H
