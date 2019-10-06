#ifndef EUPHORIA_DEFAULTFILES_H
#define EUPHORIA_DEFAULTFILES_H

#include <memory>

namespace euphoria
{
    namespace core
    {
        namespace vfs
        {
            class FileSystemRootCatalog;
        }
    }  // namespace core

    namespace render
    {
        void
        SetupDefaultFiles(
                std::shared_ptr<core::vfs::FileSystemRootCatalog> catalog);
    }
}  // namespace euphoria

#endif  // EUPHORIA_DEFAULTFILES_H
