#ifndef EUPHORIA_SDLWINDOW_H
#define EUPHORIA_SDLWINDOW_H

#include <string>

extern "C" {
struct SDL_Window;
}

class SdlWindow
{
 public:
  SdlWindow(const std::string& title, int width, int height);

  ~SdlWindow();

  void
  KeepWithin(bool k);

  void
  EnableCharEvent(bool enable);

  SDL_Window* window;
};


#endif  // EUPHORIA_SDLWINDOW_H
