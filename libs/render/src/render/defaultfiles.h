#pragma once

#include <memory>


namespace eu
{
    namespace io
    {
        struct ReadRootCatalog;
    }


    namespace render
    {
        void
        setup_default_files(std::shared_ptr<io::ReadRootCatalog> catalog);
    }
}
