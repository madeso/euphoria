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
    struct ScriptRegistry;
    struct ObjectCreator;

    void
    load_world
    (
            core::vfs::FileSystem* fs,
            core::ecs::World* world,
            ScriptRegistry* reg,
            const core::vfs::FilePath& path,
            ObjectCreator* creator,
            LuaState* ctx
    );
}
