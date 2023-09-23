#include "t3d/filelist.h"

#include "assert/assert.h"
#include "base/os.h"
#include "io/vfs.h"

#include <algorithm>


namespace eu::t3d
{
    void
    FileList::add_directory
    (
        const io::DirPath& directory,
        io::FileSystem* file_system
    )
    {
        auto listed_files = file_system->list_files(directory);
        for(const auto& relative_file: listed_files)
        {
            if(relative_file.is_file == false) { continue; }
            const auto file = directory.get_file(relative_file.name);
            const auto my_ext = file.get_extension();
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
    FileList::has_more_files() const
    {
        return !files.empty();
    }


    io::FilePath
    FileList::get_next_file()
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
