#pragma once

#include <memory>


namespace eu
{
    namespace core::vfs
    {
        struct ReadRootCatalog;
    }


    namespace render
    {
        void
        setup_default_files(std::shared_ptr<core::vfs::ReadRootCatalog> catalog);
    }
}
