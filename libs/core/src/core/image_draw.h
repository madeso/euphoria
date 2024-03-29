#pragma once

#include "core/image.h"
#include "base/rgb.h"
#include "base/rect.h"
#include "base/vec2.h"
#include "core/rgb_blend.h"




namespace eu::core
{
    struct LoadedFont;

    // get a Rect encompassing the whole image
    Recti
    on_whole_image(const Image& image);

    void
    clear(Image* image, const Rgbai& color);

    void
    draw_rect(Image* image, const Rgbai& color, const Recti& rect);

    void
    draw_square(Image* image, const Rgbai& color, int x, int y, int size);

    void
    fill_poly(Image* image, const Rgbai& color, const std::vector<vec2f>& poly);

    void
    draw_circle
    (
        Image* image,
        const Rgb& color,
        const vec2i& center,
        float radius,
        float softness = 0.0f,
        float inner = -1.0f
    );

    void
    draw_line_fast
    (
        Image* image,
        const Rgbai& color,
        const vec2i& from,
        const vec2i& to
    );

    void
    draw_line_antialiased
    (
        Image* image,
        const Rgb& color,
        const vec2f& from,
        const vec2f& to
    );

    void
    draw_line_antialiased
    (
        Image* image,
        const Rgb& color,
        const vec2i& from,
        const vec2i& to
    );

    // position is lower left of text
    void
    draw_text
    (
        Image* image,
        const vec2i& start_pos,
        const std::string& text,
        const Rgbai& color,
        const LoadedFont& font
    );

    enum class PixelsOutside
    {
        assert,
        discard
    };

    void
    paste_image
    (
        // destination: paste to this image
        Image* dest_image,
        // bottom left
        const vec2i& position,
        // source: paste from this image
        const Image& source_image,
        // how to blend
        BlendMode blend = BlendMode::normal,
        // how to handle pixels that fall outside
        PixelsOutside clip = PixelsOutside::assert
    );

    void
    draw_arrow
    (
        Image* image,
        const vec2f& from,
        const vec2f& to,
        const Rgbai& color,
        float size
    );

}
