#pragma once

#include "core/image.h"
#include "core/rgb.h"
#include "core/rect.h"
#include "core/vec2.h"
#include "core/rgb_blend.h"

#include <string>
#include <vector>

namespace euphoria::core
{
    struct LoadedFont;

    // get a rect encompassing the whole image
    recti
    whole_image(const image& image);

    void
    clear(image* image, const rgbai& color);

    void
    draw_rect(image* image, const rgbai& color, const recti& rect);

    void
    draw_square(image* image, const rgbai& color, int x, int y, int size);

    void
    fill_poly(image* image, const rgbai& color, const std::vector<vec2f>& poly);

    void
    draw_circle
    (
        image* image,
        const rgb& color,
        const vec2i& center,
        float radius,
        float softness = 0.0f,
        float inner = -1.0f
    );

    void
    draw_line_fast
    (
        image* image,
        const rgbai& color,
        const vec2i& from,
        const vec2i& to
    );

    void
    draw_line_antialiased
    (
        image* image,
        const rgb& color,
        const vec2f& from,
        const vec2f& to
    );

    void
    draw_line_antialiased
    (
        image* image,
        const rgb& color,
        const vec2i& from,
        const vec2i& to
    );

    // position is lower left of text
    void
    draw_text
    (
        image* image,
        const vec2i& start_pos,
        const std::string& text,
        const rgbai& color,
        const LoadedFont& font
    );

    enum class pixels_outside
    {
        Assert,
        Discard
    };

    void
    paste_image
    (
        // destination: paste to this image
        image* dest_image,
        // bottom left
        const vec2i& position,
        // source: paste from this image
        const image& source_image,
        // how to blend
        blend_mode blend = blend_mode::normal,
        // how to handle pixels that fall outside
        pixels_outside clip = pixels_outside::Assert
    );

    void
    draw_arrow
    (
        image* image,
        const vec2f& from,
        const vec2f& to,
        const rgbai& color,
        float size
    );

}
