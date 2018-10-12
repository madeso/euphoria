#ifndef EUPHORIA_SYSTEMS_H
#define EUPHORIA_SYSTEMS_H

class Systems;
struct Components;

namespace duk {
class Duk;
}

void
AddSystems(Systems* systems, duk::Duk* duk, Components* components);

#endif  // EUPHORIA_SYSTEMS_H
