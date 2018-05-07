#ifndef EUPHORIA_SYSTEMS_H
#define EUPHORIA_SYSTEMS_H

class Systems;
class Duk;
struct Components;

void
AddSystems(Systems* systems, Duk* duk, Components* components);

#endif  // EUPHORIA_SYSTEMS_H
