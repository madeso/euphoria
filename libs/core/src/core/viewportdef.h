#pragma once


#include "core/rect.h"

namespace euphoria::core
{
    struct ViewportDefinition
    {
        Recti screen_rect;

        float virtual_width;
        float virtual_height;

        /** The viewport is scaled, with aspect in mind, and centered.
         * Fits the viewport, scaling it, keeping the aspect ratio. Black bars may appear if the aspect ration doesnt match
         */
        [[nodiscard]]
        static ViewportDefinition from_fit_with_black_bars
        (
            float width,
            float height,
            int window_width,
            int window_height
        );

        /** The viewports height or width is extended to match the screen.
         * Fits the viewport, scaling it to the max and then fits the viewport without stretching. This means that the viewport isnt the same size as requested.
         */
        [[nodiscard]]
        static ViewportDefinition from_extend
        (
            float width,
            float height,
            int window_width,
            int window_height
        );

        /** The viewport matches the screen pixel by pixel.
         */
        [[nodiscard]] static ViewportDefinition from_screen_pixel
        (
            int window_width,
            int window_height
        );

        ViewportDefinition(const Recti& screen, float w, float h);
    };

}

