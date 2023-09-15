#pragma once

#include "core/mousebehaviour.h"
#include "core/vec2.h"


extern "C"
{
    struct SDL_Window;
}


namespace eu::window
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
        set_mouse_behaviour(core::MouseBehaviour) const;

        SdlWindow(const SdlWindow& other) = delete;
        void operator=(const SdlWindow&) = delete;
        SdlWindow(SdlWindow&& other) = delete;
        void operator=(SdlWindow&&) = delete;

        // 0-(size-1)
        [[nodiscard]] core::vec2i
        get_mouse_position() const;

        SDL_Window* window;
    };


    void
    enable_char_event(bool enable);
}
