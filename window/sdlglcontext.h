#ifndef EUPHORIA_SDLGLCONTEXT_H
#define EUPHORIA_SDLGLCONTEXT_H

#include <SDL_video.h>


namespace euphoria::window
{
    class SdlWindow;

    class SdlGlContext
    {
    public:
        SdlGlContext(SdlWindow* window);

        ~SdlGlContext();

        void* context;
    };
}  // namespace euphoria::window


#endif  // EUPHORIA_SDLGLCONTEXT_H
