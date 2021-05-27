#pragma once


#include "core/vec2.h"
#include <string>

extern "C"
{
    struct SDL_Window;
}

namespace euphoria::window
{
    struct sdl_window
    {
        sdl_window
        (
            const std::string& title,
            int width,
            int height,
            bool resize = false
        );

        ~sdl_window();

        void
        keep_within(bool k);

        void
        enable_char_event(bool enable);

        // 0-(size-1)
        const core::vec2i
        get_mouse_position() const;

        SDL_Window* window;
    };
}
