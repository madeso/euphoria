#include "window/sdlwindow.h"

#include "core/log.h"

#include <SDL2/SDL.h>

LOG_SPECIFY_DEFAULT_LOGGER("window")

SdlWindow::SdlWindow(const std::string& title, int width, int height)
    : window(nullptr)
{
  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, width, height,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  if(window == nullptr)
  {
    LOG_ERROR("Failed to create window " << SDL_GetError());
  }
}

SdlWindow::~SdlWindow()
{
  SDL_DestroyWindow(window);
}

void
SdlWindow::KeepWithin(bool k)
{
  // SDL_SetWindowGrab(window, k ? SDL_TRUE : SDL_FALSE);
  // SDL_ShowCursor(k ? SDL_DISABLE : SDL_ENABLE);
  SDL_SetRelativeMouseMode(k ? SDL_TRUE : SDL_FALSE);
}
