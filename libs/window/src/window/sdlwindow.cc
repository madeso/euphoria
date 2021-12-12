#include "window/sdlwindow.h"

#include "core/log.h"

#include "SDL.h"

namespace euphoria::window
{
    sdl_window::sdl_window
    (
        const std::string& title,
        int width,
        int height,
        bool resize
    )
        : window(nullptr)
    {
        window = SDL_CreateWindow
        (
            title.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width,
            height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | (resize ? SDL_WINDOW_RESIZABLE : 0)
        );

        if(window == nullptr)
        {
            LOG_ERROR("Failed to create window {0}", SDL_GetError());
        }
    }

    sdl_window::~sdl_window()
    {
        SDL_DestroyWindow(window);
    }

    void
    keep_mouse_within_window(bool k)
    {
        // SDL_SetWindowGrab(window, k ? SDL_TRUE : SDL_FALSE);
        // SDL_ShowCursor(k ? SDL_DISABLE : SDL_ENABLE);
        SDL_SetRelativeMouseMode(k ? SDL_TRUE : SDL_FALSE);
    }

    void
    enable_char_event(bool enable)
    {
        if(enable)
        {
            SDL_StartTextInput();
        }
        else
        {
            SDL_StopTextInput();
        }
    }

    [[nodiscard]] core::Vec2i
    sdl_window::get_mouse_position() const
    {
        core::Vec2i ret {0, 0};
        int width = 0;
        int height = 0;
        SDL_GetWindowSize(window, &width, &height);
        SDL_GetMouseState(&ret.x, &ret.y);
        ret.y = height - (ret.y + 1);
        return ret;
    }
}
