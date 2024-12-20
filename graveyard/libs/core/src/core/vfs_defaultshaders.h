#pragma once

#include "io/vfs.h"


namespace eu::core
{
    void
    add_default_shaders(io::FileSystem* fs, const io::DirPath& base);
}
