#pragma once



#include "core/sol_forward.h"


namespace eu::io
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
        io::FileSystem* fs,
        World* world,
        ScriptRegistry* reg,
        const io::FilePath& path,
        ObjectCreator* creator,
        LuaState* ctx
    );
}
