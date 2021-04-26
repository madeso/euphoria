#pragma once

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
        float virtual_width = 800.0f;
        float virtual_height = 600.0f;

        core::rectf* virtual_screen;

        ViewportHandler(Init* i, core::rectf* s);

        void
        Add(Shader* shader);

        void
        SetSize(int width, int height);

        void
        ClearBlack();

        [[nodiscard]] Viewport
        GetFullViewport() const;
    };
}
