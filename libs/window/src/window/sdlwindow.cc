#include "window/sdlwindow.h"

#include "log/log.h"

#include "SDL.h"

namespace eu::window
{
    SdlWindow::SdlWindow
    (
        const std::string& title,
        int width,
        int height,
        bool resize
    )
        : window(SDL_CreateWindow
        (
            title.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width,
            height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | (resize ? SDL_WINDOW_RESIZABLE : 0)
        ))
    {

        if(window == nullptr)
        {
            LOG_ERROR("Failed to create window {0}", SDL_GetError());
        }
    }

    SdlWindow::~SdlWindow()
    {
        SDL_DestroyWindow(window);
    }

    void
    SdlWindow::set_mouse_behaviour(core::MouseBehaviour mb) const
    {
        switch(mb)
        {
        case core::MouseBehaviour::normal:
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_SetWindowGrab(window, SDL_FALSE);
            break;
        case core::MouseBehaviour::relative_only:
            SDL_SetWindowGrab(window, SDL_FALSE);
            SDL_SetRelativeMouseMode(SDL_TRUE);
            break;
        case core::MouseBehaviour::wrap_within_window:
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_SetWindowGrab(window, SDL_TRUE);
            break;
        default:
            DIE("inavlid mouse behaviour");
        }
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

    [[nodiscard]] core::vec2i
    SdlWindow::get_mouse_position() const
    {
        core::vec2i ret {0, 0};
        int width = 0;
        int height = 0;
        SDL_GetWindowSize(window, &width, &height);
        SDL_GetMouseState(&ret.x, &ret.y);
        ret.y = height - (ret.y + 1);
        return ret;
    }
}
