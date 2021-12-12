#pragma once

#include "core/vfs.h"


namespace euphoria::core::vfs
{
    void
    add_default_shaders(FileSystem* fs, const DirPath& base);
}
