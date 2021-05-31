#pragma once

#include <vector>

#include "core/rect.h"
#include "render/viewport.h"


namespace euphoria::render
{
    struct shader;
    struct init;

    enum class viewport_type
    {
        screen_pixel, fit_with_black_bars, extend
    };

    struct viewport_handler
    {
        render::init* init;
        std::vector<shader*> shaders;

        viewport_type type = viewport_type::screen_pixel;
        int window_width = 0;
        int window_height = 0;

        // only used if type != pixel
        float virtual_width = 800.0f;
        float virtual_height = 600.0f;

        core::rectf* virtual_screen;

        viewport_handler(render::init* i, core::rectf* s);

        void
        add(shader* shader);

        void
        set_size(int width, int height);

        void
        clear_black();

        [[nodiscard]] viewport
        get_full_viewport() const;
    };
}
