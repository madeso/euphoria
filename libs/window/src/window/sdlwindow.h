#pragma once

#include <string>

#include "core/noncopyable.h"
#include "core/vec2.h"


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

        NONCOPYABLE(sdl_window);

        // 0-(size-1)
        [[nodiscard]] core::vec2i
        get_mouse_position() const;

        SDL_Window* window;
    };


    void
    enable_char_event(bool enable);

    void
    keep_mouse_within_window(bool k);
}
