#ifndef EUPHORIA_SYSTEMS_H
#define EUPHORIA_SYSTEMS_H

namespace euphoria::duk {
    class Duk;
}

namespace euphoria::core
{
    class Systems;
}

namespace euphoria::engine
{
    struct Components;

    void
    AddSystems(core::Systems* systems, duk::Duk* duk, Components* components);
}

#endif  // EUPHORIA_SYSTEMS_H
