#include "window/timer.h"

#include "SDL_timer.h"

#include "core/cint.h"

namespace euphoria::window
{
    SdlTimer::SdlTimer()
        : current_time_(SDL_GetPerformanceCounter())
        , last_time_(0)
    {
    }

    float
    SdlTimer::update()
    {
        last_time_ = current_time_;
        current_time_ = SDL_GetPerformanceCounter();

        const float dt = core::c_u64_to_float(current_time_ - last_time_) / core::c_u64_to_float(SDL_GetPerformanceFrequency());
        return dt;
    }
}
