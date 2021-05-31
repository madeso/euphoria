#pragma once

namespace euphoria::window
{
    struct sdl_window;

    struct sdl_gl_context
    {
    public:
        sdl_gl_context(sdl_window* window);

        ~sdl_gl_context();

        void* context;
    };
}
