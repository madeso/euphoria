#include "window/sdllibrary.h"

#include "core/log.h"

#include <SDL2/SDL.h>

LOG_SPECIFY_DEFAULT_LOGGER("sdl")

SdlLibrary::SdlLibrary()
    : ok(false)
{
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0)
  {
    LOG_ERROR("Failed to init SDL: " << SDL_GetError());
    return;
  }

  ok = true;
}

SdlLibrary::~SdlLibrary()
{
  SDL_Quit();
}
