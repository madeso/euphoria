#ifndef RENDER_VIEWPORT_H
#define RENDER_VIEWPORT_H

#include "core/rect.h"

namespace euphoria::render
{
  class Viewport
  {
  public:
    Viewport(const core::Recti& viewport);

    void
    Activate();  // sets the gl viewport

    float
    GetAspectRatio() const;

    const core::Recti viewport;
  };
}

#endif  // RENDER_VIEWPORT_H
