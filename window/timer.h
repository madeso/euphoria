//
// Created by gustav on 2017-08-14.
//

#ifndef EUPHORIA_TIMER_H
#define EUPHORIA_TIMER_H

#include <SDL_types.h>


namespace euphoria::window
{
    class SdlTimer
    {
        public:
        SdlTimer();

        float
        Update();

        private:
        Uint64 current_time_;
        Uint64 last_time_;
    };
}  // namespace euphoria::window

#endif  // EUPHORIA_TIMER_H
