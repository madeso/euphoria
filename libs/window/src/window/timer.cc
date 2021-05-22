#include "window/timer.h"

#include "SDL_timer.h"

namespace euphoria::window
{
    sdl_timer::sdl_timer()
        : current_time_(SDL_GetPerformanceCounter())
        , last_time_(0)
    {
    }

    float
    sdl_timer::update()
    {
        last_time_     = current_time_;
        current_time_  = SDL_GetPerformanceCounter();
        const float dt = (current_time_ - last_time_) * 1.0f
                         / SDL_GetPerformanceFrequency();
        return dt;
    }
}
