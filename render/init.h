#ifndef RENDER_INIT_H
#define RENDER_INIT_H

#include "core/rect.h"
#include "core/rgb.h"

typedef void* (*LoaderFunction)(const char* name);

// todo: come up with a better name
class Init
{
 public:
  explicit Init(LoaderFunction loader);
  ~Init();

  // todo: move to some place better
  void
  ClearScreen(const Rgb& color);

  bool ok;
};

#endif  // RENDER_INIT_H
