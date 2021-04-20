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
    WholeImage(const image& image);

    void
    Clear(image* image, const rgbai& color);

    void
    DrawRect(image* image, const rgbai& color, const Recti& rect);

    void
    DrawSquare(image* image, const rgbai& color, int x, int y, int size);

    void
    FillPoly(image* image, const rgbai& color, const std::vector<vec2f>& poly);

    void
    DrawCircle
    (
        image* image,
        const rgb& color,
        const vec2i& center,
        float radius,
        float softness = 0.0f,
        float inner = -1.0f
    );

    void
    DrawLineFast
    (
        image* image,
        const rgbai& color,
        const vec2i& from,
        const vec2i& to
    );

    void
    DrawLineAntialiased
    (
        image* image,
        const rgb& color,
        const vec2f& from,
        const vec2f& to
    );

    void
    DrawLineAntialiased
    (
        image* image,
        const rgb& color,
        const vec2i& from,
        const vec2i& to
    );

    // position is lower left of text
    void
    DrawText
    (
        image* image,
        const vec2i& start_pos,
        const std::string& text,
        const rgbai& color,
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
        image* dest_image,
        // bottom left
        const vec2i& position,
        // source: paste from this image
        const image& source_image,
        // how to blend
        BlendMode blend = BlendMode::Normal,
        // how to handle pixels that fall outside
        PixelsOutside clip = PixelsOutside::Assert
    );

    void
    DrawArrow
    (
        image* image,
        const vec2f& from,
        const vec2f& to,
        const rgbai& color,
        float size
    );

}  // namespace euphoria::core

#endif  // EUPHORIA_DRAW_H
