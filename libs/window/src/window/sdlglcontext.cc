#include "window/sdlglcontext.h"

#include "log/log.h"

#include "window/sdlwindow.h"

#include "SDL_video.h"


namespace eu::window
{
    SdlOpenglContext::SdlOpenglContext(SdlWindow* window)
        : context(SDL_GL_CreateContext(window->window))
    {
        if(context == nullptr)
        {
            LOG_ERROR("Failed to create GL context {0}", SDL_GetError());
            return;
        }
    }

    SdlOpenglContext::~SdlOpenglContext()
    {
        SDL_GL_DeleteContext(context);
    }
}
