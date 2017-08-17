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
}

void
Viewport::Activate()
{
  const vec2i top_left = viewport_.TopLeft();
  glViewport(top_left.x, top_left.y, viewport_.GetWidth(),
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
