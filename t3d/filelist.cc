#include "t3d/filelist.h"

#include <algorithm>

#include "core/assert.h"
#include "core/os.h"
#include "core/vfs.h"
#include "core/vfs_path.h"


namespace euphoria::t3d
{
    void
    FileList::AddDirectory
    (
        const std::string& directory_path,
        core::vfs::FileSystem* file_system
    )
    {
        const auto dir   = core::vfs::Path::FromDirectory(directory_path);
        auto listed_files = file_system->ListFiles(dir);
        for(const auto& relative_file: listed_files)
        {
            const auto my_ext = core::GetExtension(relative_file.name);
            const auto found_ext = std::find
            (
                extensions.begin(),
                extensions.end(),
                my_ext
            );
            const auto has_ext = found_ext != extensions.end();
            if(has_ext)
            {
                const auto file      = dir.GetFile(relative_file.name);
                const auto file_path = file.GetAbsolutePath();
                files.emplace_back(file_path);
            }
        }
    }


    bool
    FileList::HasMoreFiles() const
    {
        return !files.empty();
    }


    std::string
    FileList::NextFile()
    {
        ASSERT(!files.empty());
        ASSERTX(index < files.size(), index, files.size());
        const auto ret = files[index];
        
        index += 1;
        if(index >= files.size())
        {
            files.resize(0);
            index = 0;
        }

        return ret;
    }
}