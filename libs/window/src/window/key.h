#pragma once

#include "SDL_system.h"
#include "SDL_events.h"

#include "core/key.h"

namespace eu::window
{
    core::Key
    to_key(SDL_Keysym key);

    core::MouseButton
    to_key(SDL_MouseButtonEvent mb);
}
