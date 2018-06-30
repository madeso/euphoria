#ifndef EUPHORIA_SDLWINDOW_H
#define EUPHORIA_SDLWINDOW_H

#include "core/vec2.h"
#include <string>

extern "C" {
struct SDL_Window;
}

class SdlWindow
{
 public:
  SdlWindow(const std::string& title, int width, int height, bool resize=false);

  ~SdlWindow();

  void
  KeepWithin(bool k);

  void
  EnableCharEvent(bool enable);

  // 0-(size-1)
  const vec2i
  GetMousePosition() const;

  SDL_Window* window;
};


#endif  // EUPHORIA_SDLWINDOW_H
