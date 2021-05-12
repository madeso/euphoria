#ifndef EUPHORIA_BROWSER_H
#define EUPHORIA_BROWSER_H

#include <vector>
#include <string>
#include <optional>

#include "core/vfs.h"
#include "core/vfs_path.h"

namespace euphoria::editor
{
    struct FileBrowser
    {
        core::vfs::dir_path current_folder;
        int selected_file = -1;
        std::vector<core::vfs::listed_file> files;
        std::string filter = "";
        core::vfs::file_system* file_system = nullptr;

        explicit FileBrowser(core::vfs::file_system* fs);

        std::optional<core::vfs::file_path>
        GetSelectedFile();

        void
        SelectFile(const std::string& p);

        void
        Refresh();

        bool
        Run();
    };
}  // namespace euphoria::editor

#endif  // EUPHORIA_BROWSER_H
