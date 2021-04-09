#pragma once

#include <memory>


namespace euphoria
{
    namespace core::vfs
    {
        struct FileSystemRootCatalog;
    }


    namespace render
    {
        void
        SetupDefaultFiles(std::shared_ptr<core::vfs::FileSystemRootCatalog> catalog);
    }
}
