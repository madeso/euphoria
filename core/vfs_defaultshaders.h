#ifndef CORE_FILESYSTEMDEFAULTSHADERS_H
#define CORE_FILESYSTEMDEFAULTSHADERS_H


#include "core/vfs.h"

namespace euphoria::core
{
    namespace vfs
    {
        void AddDefaultShaders(FileSystem* fs, const std::string& base);

    }  // namespace vfs

}  // namespace euphoria::core

#endif  // CORE_FILESYSTEMDEFAULTSHADERS_H
