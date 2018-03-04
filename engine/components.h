#ifndef EUPHORIA_COMPONENTS_H
#define EUPHORIA_COMPONENTS_H

#include "core/vec2.h"
#include "render/texturecache.h"

struct CPosition2
{
  CPosition2();
  vec2f pos;

  vec2f*
  GetPositionPtr();
};

struct CSprite
{
  std::shared_ptr<Texture2d> texture;
};

#endif  // EUPHORIA_COMPONENTS_H
