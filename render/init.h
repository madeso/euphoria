#ifndef RENDER_INIT_H
#define RENDER_INIT_H

#include "core/rect.h"
#include "core/rgb.h"

typedef void* (*LoaderFunction)(const char* name);

// todo: come up with a better name
class Init
{
 public:
  enum class BlendHack
  {
    NoHack,
    EnableHack
  };

  explicit Init(
      LoaderFunction loader, BlendHack blend_hack = BlendHack::NoHack);
  ~Init();

  // todo: move to some place better
  void
  ClearScreen(const Rgb& color);

  bool ok;
};

#endif  // RENDER_INIT_H
