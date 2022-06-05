#pragma once



#include "core/rect.h"
#include "core/viewport.h"


namespace euphoria::render
{
    struct ShaderProgram;
    struct Init;

    enum class ViewportType
    {
        screen_pixel, fit_with_black_bars, extend
    };

    struct ViewportHandler
    {
        render::Init* init;
        std::vector<ShaderProgram*> shaders;

        ViewportType type = ViewportType::screen_pixel;
        int window_width = 0;
        int window_height = 0;

        // only used if type != pixel
        float virtual_width = 800.0f;
        float virtual_height = 600.0f;

        core::Rectf* virtual_screen;

        ViewportHandler(render::Init* i, core::Rectf* s);

        void
        add(ShaderProgram* shader);

        void
        set_size(int width, int height);

        void
        clear_black();

        [[nodiscard]] core::Viewport
        get_full_viewport() const;
    };
}
