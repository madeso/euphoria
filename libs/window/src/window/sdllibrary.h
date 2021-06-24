#pragma once

#include "core/noncopyable.h"

namespace euphoria::window
{
    struct sdl_library
    {
    public:
        sdl_library();
        ~sdl_library();

        NONCOPYABLE(sdl_library);

        bool ok;
    };
}
