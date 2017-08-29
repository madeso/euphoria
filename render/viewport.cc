#include "render/viewport.h"

#include "render/gl.h"

Viewport::Viewport(const Recti& viewport)
    : viewport_(viewport)
{
}

namespace  // local
{
  Viewport*&
  ActiveViewport()
  {
    static Viewport* s_ActiveViewport = nullptr;
    return s_ActiveViewport;
  }
}  // namespace

void
Viewport::Activate()
{
  const vec2i bottom_left = viewport_.BottomLeft();
  glViewport(
      bottom_left.x,
      bottom_left.y,
      viewport_.GetWidth(),
      viewport_.GetHeight());
  ActiveViewport() = this;
}

float
Viewport::GetAspectRatio() const
{
  return viewport_.GetWidth() / static_cast<float>(viewport_.GetHeight());
}

bool
Viewport::IsActive() const
{
  return ActiveViewport() == this;
}
