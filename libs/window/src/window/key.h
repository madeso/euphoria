#pragma once

#include "SDL_system.h"
#include "SDL_events.h"

#include "core/key.h"

namespace euphoria::window
{
    core::key
    to_key(SDL_Keysym key);

    core::mouse_button
    to_key(SDL_MouseButtonEvent mb);
}
