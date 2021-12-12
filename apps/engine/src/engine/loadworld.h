#pragma once

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
    struct script_registry;
    struct object_creator;

    void
    load_world
    (
            core::vfs::FileSystem* fs,
            core::ecs::World* world,
            script_registry* reg,
            const core::vfs::FilePath& path,
            object_creator* creator,
            LuaState* ctx
    );
}
