#pragma once



#include <optional>

#include "io/vfs.h"
#include "io/vfs_path.h"

namespace eu::editor
{
    struct FileBrowser
    {
        io::DirPath current_folder;
        int selected_file = -1;
        std::vector<io::ListedFile> files;
        std::string filter;
        io::FileSystem* file_system = nullptr;

        explicit FileBrowser(io::FileSystem* a_file_system);

        std::optional<io::FilePath> get_selected_file();
        
        void refresh();
        bool run();
    };
}
