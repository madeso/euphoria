#pragma once

namespace euphoria::window
{
    struct sdl_library
    {
    public:
        sdl_library();
        ~sdl_library();

        bool ok;
    };
}
