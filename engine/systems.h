#ifndef EUPHORIA_SYSTEMS_H
#define EUPHORIA_SYSTEMS_H

namespace euphoria::duk
{
    class Duk;
}

namespace euphoria::core::ecs
{
    class Systems;
}

namespace euphoria::engine
{
    struct Components;

    void
    AddSystems(
            core::ecs::Systems* systems,
            duk::Duk*           duk,
            Components*         components);
}  // namespace euphoria::engine

#endif  // EUPHORIA_SYSTEMS_H
