#ifndef RENDER_INIT_H
#define RENDER_INIT_H

#include "core/rect.h"
#include "core/rgb.h"

// todo: come up with a better name
class Init {
 public:
  Init();
  ~Init();

  // todo: move to some place better
  void ClearScreen(const Rgb& color);

  bool ok;
};

#endif  // RENDER_INIT_H
