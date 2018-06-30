#include "render/viewport.h"

#include "render/gl.h"

Viewport::Viewport(const Recti& viewport)
    : viewport(viewport)
{
}

void
Viewport::Activate()
{
  const vec2i bottom_left = viewport.BottomLeft();
  glViewport(
      bottom_left.x, bottom_left.y, viewport.GetWidth(), viewport.GetHeight());
}

float
Viewport::GetAspectRatio() const
{
  return viewport.GetWidth() / static_cast<float>(viewport.GetHeight());
}
