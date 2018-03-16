#ifndef EUPHORIA_COMPONENTS_H
#define EUPHORIA_COMPONENTS_H

#include <memory>
#include "core/vec2.h"
#include "render/texturecache.h"

struct CPosition2
{
  CPosition2();
  vec2f pos;

  std::shared_ptr<vec2f>
  GetPosition();

  void
  SetPosition(vec2f* v);
};

struct CSprite
{
  std::shared_ptr<Texture2d> texture;
};

#endif  // EUPHORIA_COMPONENTS_H
