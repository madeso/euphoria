#include "t3d/filelist.h"

#include "core/assert.h"
#include "core/os.h"
#include "core/vfs.h"

#include <algorithm>


namespace euphoria::t3d
{
    void
    FileList::AddDirectory
    (
        const core::vfs::DirPath& directory,
        core::vfs::FileSystem* file_system
    )
    {
        auto listed_files = file_system->ListFiles(directory);
        for(const auto& relative_file: listed_files)
        {
            if(relative_file.is_file == false) { continue; }
            const auto file      = directory.GetFile(relative_file.name);
            const auto my_ext = file.GetExtension();
            const auto found_ext = std::find
            (
                extensions.begin(),
                extensions.end(),
                my_ext
            );
            const auto has_ext = found_ext != extensions.end();
            if(has_ext)
            {
                files.emplace_back(file);
            }
        }
    }


    bool
    FileList::HasMoreFiles() const
    {
        return !files.empty();
    }


    core::vfs::FilePath
    FileList::NextFile()
    {
        ASSERT(!files.empty());
        ASSERTX(index < files.size(), index, files.size());
        const auto ret = files[index];
        
        index += 1;
        if(index >= files.size())
        {
            files.clear();
            index = 0;
        }

        return ret;
    }
}