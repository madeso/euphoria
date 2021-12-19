#pragma once

#include "SDL_types.h"


namespace euphoria::window
{
    struct SdlTimer
    {
        SdlTimer();

        float
        update();

        Uint64 current_time;
        Uint64 last_time;
    };
}
