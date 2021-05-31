#pragma once

#include "SDL_types.h"


namespace euphoria::window
{
    struct sdl_timer
    {
        sdl_timer();

        float
        update();

        Uint64 current_time_;
        Uint64 last_time_;
    };
}
