#pragma once



#include "core/sol_forward.h"


namespace eu::core::vfs
{
    struct FileSystem;
    struct FilePath;
}


namespace eu::runner
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
