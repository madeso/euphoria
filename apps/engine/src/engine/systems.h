#pragma once

#include "core/sol_forward.h"


namespace euphoria::core::ecs
{
    struct Systems;
}

namespace euphoria::engine
{
    struct components;

	void
	add_systems(core::ecs::Systems* sys, components* comps);
}
