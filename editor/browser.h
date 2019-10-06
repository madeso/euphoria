#ifndef EUPHORIA_BROWSER_H
#define EUPHORIA_BROWSER_H

#include <vector>
#include <string>

#include "core/vfs.h"

namespace euphoria::editor
{
    struct FileBrowser
    {
        std::string                        current_folder;
        int                                selected_file = -1;
        std::vector<core::vfs::ListedFile> files;
        std::string                        filter      = "";
        core::vfs::FileSystem*             file_system = nullptr;

        explicit FileBrowser(core::vfs::FileSystem* fs);

        std::string
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
