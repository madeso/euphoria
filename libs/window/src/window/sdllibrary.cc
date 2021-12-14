#include "window/sdllibrary.h"

#include "core/log.h"

#include "SDL.h"

namespace euphoria::window
{
    void
    setup_sdl_open_gl_attributes()
    {
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 4);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 4);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 4);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 4);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 0);
        // for 2d we have keept this at 0
        // todo(Gustav): fix 2d rendering to we can keep this at != 0
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_STEREO, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    }

    SdlLibrary::SdlLibrary()
        : ok(false)
    {
        if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0)
        {
            LOG_ERROR("Failed to init SDL: {0}", SDL_GetError());
            return;
        }

        setup_sdl_open_gl_attributes();
        ok = true;
    }

    SdlLibrary::~SdlLibrary()
    {
        SDL_Quit();
    }
}
