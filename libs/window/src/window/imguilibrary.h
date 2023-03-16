#pragma once

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

        Library(const Library& other) = delete;
        void operator=(const Library&) = delete;
        Library(Library&& other) = delete;
        void operator=(Library&&) = delete;
    private:
        std::string path_to_imgui_ini;
    };

    void
    process_imgui_events(const SDL_Event*);

    void
    start_new_frame();

    void
    imgui_render();
}
