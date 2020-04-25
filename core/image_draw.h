#ifndef EUPHORIA_DRAW_H
#define EUPHORIA_DRAW_H

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

    Recti
    WholeImage(const Image& image);

    void
    Clear(Image* image, const Rgbai& color);

    void
    DrawRect(Image* image, const Rgbai& color, const Recti& rect);

    void
    DrawSquare(Image* image, const Rgbai& color, int x, int y, int size);

    void
    FillPoly(Image* image, const Rgbai& color, std::vector<vec2f>& poly);

    void
    DrawCircle
    (
        Image* image,
        const Rgb& color,
        const vec2i& center,
        float radius,
        float softness = 0.0f,
        float inner = -1.0f
    );

    void
    DrawLineFast
    (
        Image* image,
        const Rgbai& color,
        const vec2i& from,
        const vec2i& to
    );

    void
    DrawLineAntialiased
    (
        Image* image,
        const Rgb& color,
        const vec2f& from,
        const vec2f& to
    );

    void
    DrawLineAntialiased
    (
        Image* image,
        const Rgb& color,
        const vec2i& from,
        const vec2i& to
    );

    // position is lower left of text
    void
    DrawText
    (
        Image* image,
        const vec2i& start_pos,
        const std::string& text,
        const Rgbai& color,
        const LoadedFont& font
    );

    enum class PixelsOutside
    {
        Assert,
        Discard
    };

    void
    PasteImage
    (
        // destination: paste to this image
        Image* dest_image,
        // bottom left
        const vec2i& position,
        // source: paste from this image
        const Image& source_image,
        // how to blend
        BlendMode blend = BlendMode::Normal,
        // how to handle pixels that fall outside
        PixelsOutside clip = PixelsOutside::Assert
    );

    void
    DrawArrow
    (
        Image* image,
        const vec2f& from,
        const vec2f& to,
        const Rgbai& color,
        int size
    );

}  // namespace euphoria::core

#endif  // EUPHORIA_DRAW_H
