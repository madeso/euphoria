#pragma once

#include "core/sol_forward.h"


namespace euphoria::core::ecs
{
    struct systems;
}

namespace euphoria::engine
{
    struct components;

	void
	add_systems(core::ecs::systems* systems, Sol* duk, components* components);
}
