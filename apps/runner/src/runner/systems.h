#pragma once

#include "core/sol_forward.h"


namespace eu::runner
{
    struct Components;
    struct Systems;

	void
	add_systems(Systems* sys, Components* comps);
}
