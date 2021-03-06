#pragma once

#include <memory>


namespace euphoria
{
    namespace core::vfs
    {
        struct read_root_catalog;
    }


    namespace render
    {
        void
        setup_default_files(std::shared_ptr<core::vfs::read_root_catalog> catalog);
    }
}
