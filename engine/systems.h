#ifndef EUPHORIA_SYSTEMS_H
#define EUPHORIA_SYSTEMS_H

#include "core/sol_forward.h"

class Systems;
struct Components;

void
AddSystems(Systems* systems, Sol* duk, Components* components);

#endif  // EUPHORIA_SYSTEMS_H
