#pragma once

#include <string>

#include "window/fakesdlevent.h"

namespace euphoria::window
{
    struct sdl_gl_context;
}

namespace euphoria::window::imgui
{
    struct library
    {
    public:
        library(SDL_Window* the_window, sdl_gl_context* context, const std::string& the_path);
        ~library();

        void
        process_events(const SDL_Event*);

        void
        start_new_frame();

        void
        render();

    private:
        SDL_Window* sdl_window;
        std::string path_to_imgui_ini;
    };
}
