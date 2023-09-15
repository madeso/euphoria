#pragma once

#include "SDL_types.h"


namespace eu::window
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
