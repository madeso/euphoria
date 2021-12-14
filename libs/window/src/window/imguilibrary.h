#pragma once

#include <string>

#include "core/noncopyable.h"

#include "window/fakesdlevent.h"


namespace euphoria::window
{
    struct SdlAndOpenglContext;
}

namespace euphoria::window::imgui
{
    struct Library
    {
    public:
        Library(SDL_Window* the_window, SdlAndOpenglContext* context, const std::string& the_path);
        ~Library();

        NONCOPYABLE(Library);

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
