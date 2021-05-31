#pragma once

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
    struct script_registry;
    struct object_creator;

    void
    load_world
    (
            core::vfs::file_system* fs,
            core::ecs::world* world,
            script_registry* reg,
            const core::vfs::file_path& path,
            object_creator* creator,
            Sol* ctx
    );
}
