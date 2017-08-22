#ifndef EUPHORIA_SDLGLCONTEXT_H
#define EUPHORIA_SDLGLCONTEXT_H

#include <SDL_video.h>

class SdlWindow;

class SdlGlContext
{
 public:
  SdlGlContext(SdlWindow* window);

  ~SdlGlContext();

  void* context;
};


#endif  // EUPHORIA_SDLGLCONTEXT_H
