#ifndef EUPHORIA_LOADWORLD_H
#define EUPHORIA_LOADWORLD_H

#include <string>

#include "core/sol_forward.h"

namespace euphoria::core::ecs
{
    struct world;
}


namespace euphoria::core::vfs
{
    struct file_system;
    struct file_path;
}


namespace euphoria::engine
{
    struct DukRegistry;
    struct ObjectCreator;

    void
    LoadWorld
    (
        core::vfs::file_system* fs,
        core::ecs::world* world,
        DukRegistry* reg,
        const core::vfs::file_path& path,
        ObjectCreator* creator,
        Sol* ctx
    );
}  // namespace euphoria::engine

#endif  // EUPHORIA_LOADWORLD_H
