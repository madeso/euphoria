#ifndef RENDER_INIT_H
#define RENDER_INIT_H

#include "core/rect.h"

// todo: come up with a better name
class Init {
 public:
  Init();
  ~Init();

  void SetViewport(const Recti& viewport);

  // todo: move to some place better
  void ClearScreen();

  bool ok;
};

#endif  // RENDER_INIT_H
