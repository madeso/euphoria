#include "t3d/filelist.h"

#include "core/assert.h"
#include "core/os.h"
#include "core/vfs.h"

#include <algorithm>


namespace euphoria::t3d
{
    void
    file_list::add_directory
    (
        const core::vfs::dir_path& directory,
        core::vfs::file_system* file_system
    )
    {
        auto listed_files = file_system->list_files(directory);
        for(const auto& relative_file: listed_files)
        {
            if(relative_file.is_file == false) { continue; }
            const auto file      = directory.get_file(relative_file.name);
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
    file_list::has_more_files() const
    {
        return !files.empty();
    }


    core::vfs::file_path
    file_list::get_next_file()
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
