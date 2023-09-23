#pragma once

#include "io/vfs_path.h"





namespace eu::io
{
    struct FileSystem;
}


namespace eu::t3d
{
    struct FileList
    {
        void
        add_directory
        (
            const io::DirPath& path,
            io::FileSystem* file_system
        );


        [[nodiscard]] bool
        has_more_files() const;


        io::FilePath
        get_next_file();


        std::vector<std::string> extensions;
        std::vector<io::FilePath> files;
        std::size_t index = 0;
    };
}
