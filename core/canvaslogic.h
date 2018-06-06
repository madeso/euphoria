#ifndef EUPHORIA_CANVASLOGIC_H
#define EUPHORIA_CANVASLOGIC_H

#include "core/vec2.h"
#include "core/range.h"

class CanvasLogic
{
 public:
  // canvas view
  vec2f scroll = vec2f{0, 0};
  float scale  = 1;

  // "config"
  Range scale_range = Range{0.1f, 15.0f};

  void
  Pan(const vec2f& p);

  void
  Zoom(const vec2f& mouse, float zoom);

  vec2f
  ScreenToWorld(const vec2f& p) const;

  vec2f
  WorldToScreen(const vec2f& p) const;
};


#endif  // EUPHORIA_CANVASLOGIC_H
