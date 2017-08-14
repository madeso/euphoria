//
// Created by gustav on 2017-08-14.
//

#ifndef EUPHORIA_TIMER_H
#define EUPHORIA_TIMER_H

#include <SDL_types.h>

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



#endif //EUPHORIA_TIMER_H
