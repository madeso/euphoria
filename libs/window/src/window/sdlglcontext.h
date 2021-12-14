#pragma once

#include "core/noncopyable.h"

namespace euphoria::window
{
    struct SdlWindow;

    struct SdlAndOpenglContext
    {
    public:
        SdlAndOpenglContext(SdlWindow* window);
        ~SdlAndOpenglContext();

        NONCOPYABLE(SdlAndOpenglContext);

        void* context;
    };
}
