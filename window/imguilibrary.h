#ifndef EUPHORIA_IMGUILIBRARY_H
#define EUPHORIA_IMGUILIBRARY_H

#include <string>

#include "window/fakesdlevent.h"

namespace euphoria::window
{
    struct SdlGlContext;
    
    struct ImguiLibrary
    {
    public:
        ImguiLibrary(SDL_Window* window, SdlGlContext* context, const std::string& path);
        ~ImguiLibrary();

        void
        ProcessEvents(SDL_Event*);

        void
        StartNewFrame();

        void
        Render();

    private:
        SDL_Window* window_;
        std::string path_;
    };
}  // namespace euphoria::window

#endif  // EUPHORIA_IMGUILIBRARY_H
