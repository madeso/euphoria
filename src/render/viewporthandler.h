#ifndef EUPHORIA_RENDER_VIEWPORTHANDLER_H
#define EUPHORIA_RENDER_VIEWPORTHANDLER_H

#include <vector>

#include "core/rect.h"
#include "render/viewport.h"


namespace euphoria::render
{
    struct Shader;
    struct Init;

    enum class ViewportType
    {
        ScreenPixel, FitWithBlackBars, Extend
    };

    struct ViewportHandler
    {
        Init* init;
        std::vector<Shader*> shaders;

        ViewportType type = ViewportType::ScreenPixel;
        int window_width = 0;
        int window_height = 0;

        // only used if type != pixel
        int virtual_width = 800;
        int virtual_height = 600;

        core::Rectf* virtual_screen;

        ViewportHandler(Init* i, core::Rectf* s);

        void
        Add(Shader* shader);

        void
        SetSize(int width, int height);

        void
        ClearBlack();

        Viewport
        GetFullViewport();
    };
}


#endif  // EUPHORIA_RENDER_VIEWPORTHANDLER_H

