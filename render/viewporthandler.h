#ifndef EUPHORIA_RENDER_VIEWPORTHANDLER_H
#define EUPHORIA_RENDER_VIEWPORTHANDLER_H

#include "render/viewport.h"

namespace euphoria::render
{
    struct ViewportHandler
    {
        int width = 0;
        int height = 0;

        Viewport
        GetFullViewport();

        void
        SetSize(int new_width, int new_height);
    };
}

#endif  // EUPHORIA_RENDER_VIEWPORTHANDLER_H
