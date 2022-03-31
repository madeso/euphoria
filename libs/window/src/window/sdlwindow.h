#pragma once

#include <string>

#include "core/mousebehaviour.h"
#include "core/noncopyable.h"
#include "core/vec2.h"


extern "C"
{
    struct SDL_Window;
}


namespace euphoria::window
{
    struct SdlWindow
    {
        SdlWindow
        (
            const std::string& title,
            int width,
            int height,
            bool resize = false
        );

        ~SdlWindow();

        void
        set_mouse_behaviour(core::MouseBehaviour);

        NONCOPYABLE(SdlWindow);

        // 0-(size-1)
        [[nodiscard]] core::Vec2i
        get_mouse_position() const;

        SDL_Window* window;
    };


    void
    enable_char_event(bool enable);
}
