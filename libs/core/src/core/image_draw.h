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

    // get a Rect encompassing the whole image
    Recti
    whole_image(const Image& image);

    void
    clear(Image* image, const Rgbai& color);

    void
    draw_rect(Image* image, const Rgbai& color, const Recti& rect);

    void
    draw_square(Image* image, const Rgbai& color, int x, int y, int size);

    void
    fill_poly(Image* image, const Rgbai& color, const std::vector<Vec2f>& poly);

    void
    draw_circle
    (
        Image* image,
        const Rgb& color,
        const Vec2i& center,
        float radius,
        float softness = 0.0f,
        float inner = -1.0f
    );

    void
    draw_line_fast
    (
        Image* image,
        const Rgbai& color,
        const Vec2i& from,
        const Vec2i& to
    );

    void
    draw_line_antialiased
    (
        Image* image,
        const Rgb& color,
        const Vec2f& from,
        const Vec2f& to
    );

    void
    draw_line_antialiased
    (
        Image* image,
        const Rgb& color,
        const Vec2i& from,
        const Vec2i& to
    );

    // position is lower left of text
    void
    draw_text
    (
        Image* image,
        const Vec2i& start_pos,
        const std::string& text,
        const Rgbai& color,
        const LoadedFont& font
    );

    enum class pixels_outside
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
        const Vec2i& position,
        // source: paste from this image
        const Image& source_image,
        // how to blend
        BlendMode blend = BlendMode::normal,
        // how to handle pixels that fall outside
        pixels_outside clip = pixels_outside::assert
    );

    void
    draw_arrow
    (
        Image* image,
        const Vec2f& from,
        const Vec2f& to,
        const Rgbai& color,
        float size
    );

}
