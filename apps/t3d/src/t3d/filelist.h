#ifndef EUPHORIA_T3D_FILELIST_H
#define EUPHORIA_T3D_FILELIST_H

#include "core/vfs_path.h"

#include <string>
#include <vector>


namespace euphoria::core::vfs
{
    struct file_system;
}


namespace euphoria::t3d
{
    struct FileList
    {
        void
        AddDirectory
        (
            const core::vfs::dir_path& path,
            core::vfs::file_system* file_system
        );


        bool
        HasMoreFiles() const;


        core::vfs::file_path
        NextFile();


        std::vector<std::string> extensions;
        std::vector<core::vfs::file_path> files;
        std::size_t index = 0;
    };
}

#endif  // EUPHORIA_T3D_FILELIST_H
