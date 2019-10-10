#ifndef EUPHORIA_LOADWORLD_H
#define EUPHORIA_LOADWORLD_H

#include <string>

namespace euphoria::core::ecs
{
    struct World;
}

namespace euphoria::core::vfs
{
    struct FileSystem;
}

namespace euphoria::duk
{
    struct Context;
    struct Duk;
}  // namespace euphoria::duk

namespace euphoria::engine
{
    struct DukRegistry;
    struct ObjectCreator;

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
