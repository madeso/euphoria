#ifndef RENDER_VIEWPORT_H
#define RENDER_VIEWPORT_H

#include "core/rect.h"

class Viewport {
 public:
  Viewport(const Recti& viewport);

  void Activate(); // sets the gl viewport
  float GetAspectRatio() const;

  // debug
  bool IsActive() const;

 private:
  Recti viewport_;

};

#endif  // RENDER_VIEWPORT_H
