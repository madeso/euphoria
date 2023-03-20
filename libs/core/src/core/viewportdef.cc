#include "core/viewportdef.h"

#include "core/cint.h"


namespace euphoria::core
{
    ViewportDefinition
    ViewportDefinition::from_fit_with_black_bars
    (
        float width,
        float height,
        int window_width,
        int window_height
    )
    {
        ASSERTX(width > 0, width);
        ASSERTX(height > 0, height);
        ASSERTX(window_width >= 0, window_width);
        ASSERTX(window_height >= 0, window_height);
        const float w = static_cast<float>(window_width) / width;
        const float h = static_cast<float>(window_height) / height;
        const float s = min(w, h);
        ASSERTX(s > 0, s, w, h);
        const float new_width = width * s;
        const float new_height = height * s;
        return ViewportDefinition
        {
            Recti::from_width_height
            (
                c_float_to_int(new_width),
                c_float_to_int(new_height)
            ).set_bottom_left_to_copy
            (
                c_float_to_int((c_int_to_float(window_width) - new_width) / 2.0f),
                c_float_to_int((c_int_to_float(window_height) - new_height) / 2.0f)
            ),
            width,
            height
        };
    }


    float
    determine_extend_scale(float scale, float height, int window_height)
    {
        const auto scaled_height = height * scale;
        const auto s = static_cast<float>(window_height) / scaled_height;
        return s;
    }


    ViewportDefinition
    ViewportDefinition::from_extend
    (
        float width,
        float height,
        int window_width,
        int window_height
    )
    {
        ASSERTX(width >= 0, width);
        ASSERTX(height >= 0, height);
        ASSERTX(window_width >= 0, window_width);
        ASSERTX(window_height >= 0, window_height);
        const auto w = static_cast<float>(window_width) / width;
        const auto h = static_cast<float>(window_height) / height;
        const auto r = Recti::from_width_height(window_width, window_height)
                               .set_bottom_left_to_copy(0, 0);
        if(w < h)
        {
            const auto s = determine_extend_scale(w, height, window_height);
            return ViewportDefinition {r, width, height * s};
        }
        else
        {
            const auto s = determine_extend_scale(h, width, window_width);
            return ViewportDefinition {r, width * s, height};
        }
    }


    ViewportDefinition
    ViewportDefinition::from_screen_pixel(int window_width, int window_height)
    {
        ASSERTX(window_width >= 0, window_width);
        ASSERTX(window_height >= 0, window_height);

        return ViewportDefinition
        {
            Recti::from_width_height(window_width, window_height)
                    .set_bottom_left_to_copy(0, 0),
            static_cast<float>(window_width),
            static_cast<float>(window_height)
        };
    }


    ViewportDefinition::ViewportDefinition(const Recti& screen, float w, float h)
        : screen_rect(screen)
        , virtual_width(w)
        , virtual_height(h)
    {
    }
}

