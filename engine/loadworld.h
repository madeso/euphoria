#ifndef EUPHORIA_LOADWORLD_H
#define EUPHORIA_LOADWORLD_H

#include <string>

namespace euphoria::core::ecs
{
    class World;
}

namespace euphoria::core::vfs
{
    class FileSystem;
}

namespace euphoria::duk
{
    class Context;
    class Duk;
}  // namespace euphoria::duk

namespace euphoria::engine
{
    class DukRegistry;
    class ObjectCreator;

    void
    LoadWorld(
            core::vfs::FileSystem* fs,
            core::ecs::World*      world,
            DukRegistry*           reg,
            const std::string&     path,
            ObjectCreator*         creator,
            duk::Context*          ctx,
            duk::Duk*              duk);
}  // namespace euphoria::engine

#endif  // EUPHORIA_LOADWORLD_H
