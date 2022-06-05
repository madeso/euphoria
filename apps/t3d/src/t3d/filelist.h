#pragma once

#include "core/vfs_path.h"





namespace euphoria::core::vfs
{
    struct FileSystem;
}


namespace euphoria::t3d
{
    struct FileList
    {
        void
        add_directory
        (
            const core::vfs::DirPath& path,
            core::vfs::FileSystem* file_system
        );


        [[nodiscard]] bool
        has_more_files() const;


        core::vfs::FilePath
        get_next_file();


        std::vector<std::string> extensions;
        std::vector<core::vfs::FilePath> files;
        std::size_t index = 0;
    };
}
