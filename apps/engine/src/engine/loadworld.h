#ifndef EUPHORIA_LOADWORLD_H
#define EUPHORIA_LOADWORLD_H

#include <string>

#include "core/sol_forward.h"

namespace euphoria::core::ecs
{
    struct World;
}


namespace euphoria::core::vfs
{
    struct FileSystem;
    struct FilePath;
}


namespace euphoria::engine
{
    struct DukRegistry;
    struct ObjectCreator;

    void
    LoadWorld
    (
        core::vfs::FileSystem* fs,
        core::ecs::World* world,
        DukRegistry* reg,
        const core::vfs::FilePath& path,
        ObjectCreator* creator,
        Sol* ctx
    );
}  // namespace euphoria::engine

#endif  // EUPHORIA_LOADWORLD_H
