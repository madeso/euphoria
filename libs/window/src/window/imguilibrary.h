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
        start_new_frame();

    private:
        SDL_Window* sdl_window;
        std::string path_to_imgui_ini;
    };

    void
    process_imgui_events(const SDL_Event*);

    void
    imgui_render();
}
