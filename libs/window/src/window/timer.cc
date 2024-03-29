#include "window/timer.h"

#include "SDL_timer.h"

#include "base/cint.h"

namespace eu::window
{
    SdlTimer::SdlTimer()
        : current_time(SDL_GetPerformanceCounter())
        , last_time(0)
    {
    }

    float
    SdlTimer::update()
    {
        last_time = current_time;
        current_time = SDL_GetPerformanceCounter();

        const float dt = c_u64_to_float(current_time - last_time) / c_u64_to_float(SDL_GetPerformanceFrequency());
        return dt;
    }
}
