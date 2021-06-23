#pragma once

#include "core/vfs_path.h"

#include <string>
#include <vector>


namespace euphoria::core::vfs
{
    struct file_system;
}


namespace euphoria::t3d
{
    struct file_list
    {
        void
        add_directory
        (
            const core::vfs::dir_path& path,
            core::vfs::file_system* file_system
        );


        [[nodiscard]] bool
        has_more_files() const;


        core::vfs::file_path
        get_next_file();


        std::vector<std::string> extensions;
        std::vector<core::vfs::file_path> files;
        std::size_t index = 0;
    };
}
