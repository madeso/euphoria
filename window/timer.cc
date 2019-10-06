#include "window/timer.h"

#include <SDL_timer.h>

namespace euphoria::window
{
    SdlTimer::SdlTimer()
        : current_time_(SDL_GetPerformanceCounter()), last_time_(0)
    {}

    float
    SdlTimer::Update()
    {
        last_time_     = current_time_;
        current_time_  = SDL_GetPerformanceCounter();
        const float dt = (current_time_ - last_time_) * 1.0f
                         / SDL_GetPerformanceFrequency();
        return dt;
    }
}  // namespace euphoria::window
