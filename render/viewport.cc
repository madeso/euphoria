#include "render/viewport.h"

#include "render/gl.h"

Viewport::Viewport(const Recti& viewport)
    : viewport(viewport)
{
}

namespace  // local
{
  Viewport*&
  ActiveViewport()
  {
    static Viewport* ActiveViewport = nullptr;
    return ActiveViewport;
  }
}  // namespace

void
Viewport::Activate()
{
  const vec2i bottom_left = viewport.BottomLeft();
  glViewport(
      bottom_left.x, bottom_left.y, viewport.GetWidth(), viewport.GetHeight());
  ActiveViewport() = this;
}

float
Viewport::GetAspectRatio() const
{
  return viewport.GetWidth() / static_cast<float>(viewport.GetHeight());
}

bool
Viewport::IsActive() const
{
  return ActiveViewport() == this;
}
