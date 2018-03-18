#ifndef EUPHORIA_WINDOW_KEY_H
#define EUPHORIA_WINDOW_KEY_H

#include <SDL_system.h>
#include <SDL_events.h>

#include "core/key.h"

Key
ToKey(SDL_Keysym key);

MouseButton
ToKey(SDL_MouseButtonEvent mb);

#endif  // EUPHORIA_WINDOW_KEY_H
