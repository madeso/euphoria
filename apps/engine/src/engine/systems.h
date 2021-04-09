#ifndef EUPHORIA_SYSTEMS_H
#define EUPHORIA_SYSTEMS_H

#include "core/sol_forward.h"


namespace euphoria::core::ecs
{
    struct Systems;
}

namespace euphoria::engine
{
    struct Components;

	void
	AddSystems(core::ecs::Systems* systems, Sol* duk, Components* components);
}  // namespace euphoria::engine

#endif  // EUPHORIA_SYSTEMS_H
