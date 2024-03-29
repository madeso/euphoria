#pragma once

#include "window/fakesdlevent.h"


namespace eu::window
{
    struct SdlOpenglContext;
}

namespace eu::window::imgui
{
    struct Library
    {
    public:
        Library(SDL_Window* the_window, SdlOpenglContext* context, const std::string& the_path);
        ~Library();

        Library(const Library& other) = delete;
        void operator=(const Library&) = delete;
        Library(Library&& other) = delete;
        void operator=(Library&&) = delete;
    private:
        std::string path_to_imgui_ini;
    };

    void
    send_events_to_imgui(const SDL_Event*);

    void
    begin_new_frame();

    void
    imgui_render();
}
