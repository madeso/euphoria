#include "window/sdlglcontext.h"

#include "core/log.h"

#include "window/sdlwindow.h"

#include <SDL.h>


namespace euphoria::window
{
    LOG_SPECIFY_DEFAULT_LOGGER("sdl.gl-context")

    SdlGlContext::SdlGlContext(SdlWindow* window) : context(nullptr)
    {
        context = SDL_GL_CreateContext(window->window);
        if(context == nullptr)
        {
            LOG_ERROR("Failed to create GL context " << SDL_GetError());
            return;
        }
    }

    SdlGlContext::~SdlGlContext()
    {
        SDL_GL_DeleteContext(context);
    }
}  // namespace euphoria::window
