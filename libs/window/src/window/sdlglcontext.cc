#include "window/sdlglcontext.h"

#include "core/log.h"

#include "window/sdlwindow.h"

#include "SDL_video.h"


namespace euphoria::window
{
    SdlAndOpenglContext::SdlAndOpenglContext(SdlWindow* window) : context(nullptr)
    {
        context = SDL_GL_CreateContext(window->window);
        if(context == nullptr)
        {
            LOG_ERROR("Failed to create GL context {0}", SDL_GetError());
            return;
        }
    }

    SdlAndOpenglContext::~SdlAndOpenglContext()
    {
        SDL_GL_DeleteContext(context);
    }
}
