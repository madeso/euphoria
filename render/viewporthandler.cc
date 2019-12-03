#include "render/viewporthandler.h"

#include "render/viewport.h"

namespace euphoria::render
{
    void
    ViewportHandler::SetSize(int new_width, int new_height)
    {
        width = new_width;
        height = new_height;

        const auto viewport = GetFullViewport();
        viewport.Activate();
    }

    Viewport
    ViewportHandler::GetFullViewport()
    {
        Viewport viewport {core::Recti::FromWidthHeight(width, height)
                                .SetBottomLeftToCopy(0, 0)};
        return viewport;
    }
}