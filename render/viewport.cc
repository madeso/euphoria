#include "render/viewport.h"

#include "render/gl.h"

namespace euphoria::render
{

  Viewport::Viewport(const core::Recti& viewport)
      : viewport(viewport)
  {
  }

  void
  Viewport::Activate()
  {
    const auto bottom_left = viewport.BottomLeft();
    glViewport(
        bottom_left.x, bottom_left.y, viewport.GetWidth(), viewport.GetHeight());

    // some gl funcs doesnt respect viewport, so we need to enable
    // scissor testing too
    glScissor(
        bottom_left.x, bottom_left.y, viewport.GetWidth(), viewport.GetHeight());
  }

  float
  Viewport::GetAspectRatio() const
  {
    return viewport.GetWidth() / static_cast<float>(viewport.GetHeight());
  }

}
