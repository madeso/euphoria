#ifndef EUPHORIA_T3D_FILELIST_H
#define EUPHORIA_T3D_FILELIST_H

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
            const std::string& path,
            core::vfs::FileSystem* file_system
        );


        bool
        HasMoreFiles() const;


        std::string
        NextFile();


        std::vector<std::string> extensions;
        std::vector<std::string> files;
        std::size_t index = 0;
    };
}

#endif  // EUPHORIA_T3D_FILELIST_H
