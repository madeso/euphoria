#ifndef EUPHORIA_T3D_FILELIST_H
#define EUPHORIA_T3D_FILELIST_H

#include "core/vfs_path.h"

#include <string>
#include <vector>


namespace euphoria::core::vfs
{
    struct FileSystem;
}


namespace euphoria::t3d
{
    struct FileList
    {
        void
        AddDirectory
        (
            const core::vfs::DirPath& path,
            core::vfs::FileSystem* file_system
        );


        bool
        HasMoreFiles() const;


        core::vfs::FilePath
        NextFile();


        std::vector<std::string> extensions;
        std::vector<core::vfs::FilePath> files;
        std::size_t index = 0;
    };
}

#endif  // EUPHORIA_T3D_FILELIST_H
