#ifndef EUPHORIA_WINDOW_KEY_H
#define EUPHORIA_WINDOW_KEY_H

#include <SDL_system.h>
#include <SDL_events.h>

#include "core/key.h"

namespace euphoria::window
{
    core::Key
    ToKey(SDL_Keysym key);

    core::MouseButton
    ToKey(SDL_MouseButtonEvent mb);
}  // namespace euphoria::window

#endif  // EUPHORIA_WINDOW_KEY_H
