#pragma once



#include <optional>

#include "core/vfs.h"
#include "core/vfs_path.h"

namespace eu::editor
{
    struct FileBrowser
    {
        core::vfs::DirPath current_folder;
        int selected_file = -1;
        std::vector<core::vfs::ListedFile> files;
        std::string filter;
        core::vfs::FileSystem* file_system = nullptr;

        explicit FileBrowser(core::vfs::FileSystem* a_file_system);

        std::optional<core::vfs::FilePath> get_selected_file();
        
        void refresh();
        bool run();
    };
}
