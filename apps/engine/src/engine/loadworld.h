#pragma once

#include <string>

#include "core/sol_forward.h"


namespace euphoria::core::vfs
{
    struct FileSystem;
    struct FilePath;
}


namespace euphoria::engine
{
    struct ScriptRegistry;
    struct ObjectCreator;
    struct World;

    void
    load_world
    (
        core::vfs::FileSystem* fs,
        World* world,
        ScriptRegistry* reg,
        const core::vfs::FilePath& path,
        ObjectCreator* creator,
        LuaState* ctx
    );
}
