#pragma once

#include <vector>
#include <string>
#include <optional>

#include "core/vfs.h"
#include "core/vfs_path.h"

namespace euphoria::editor
{
    struct file_browser
    {
        core::vfs::dir_path current_folder;
        int selected_file = -1;
        std::vector<core::vfs::listed_file> files;
        std::string filter;
        core::vfs::file_system* file_system = nullptr;

        explicit file_browser(core::vfs::file_system* fs);

        std::optional<core::vfs::file_path>
        get_selected_file();

        void
        refresh();

        bool
        run();
    };
}
