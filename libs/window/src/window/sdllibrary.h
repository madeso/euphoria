#pragma once

#include "core/noncopyable.h"

namespace euphoria::window
{
    struct SdlLibrary
    {
    public:
        SdlLibrary();
        ~SdlLibrary();

        NONCOPYABLE(SdlLibrary);

        bool ok;
    };
}
