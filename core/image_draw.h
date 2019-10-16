#ifndef EUPHORIA_DRAW_H
#define EUPHORIA_DRAW_H

#include "core/image.h"
#include "core/rgb.h"
#include "core/rect.h"
#include "core/vec2.h"

#include <string>

namespace euphoria::core
{

    Recti
    WholeImage(const Image& image);

    void
    Clear(Image* image, const Rgbi& color);

    void
    DrawRect(Image* image, const Rgbi& color, const Recti& rect);

    void
    DrawSquare(Image* image, const Rgbi& color, int x, int y, int size);

    void
    DrawCircle(Image* image, const Rgb&   color,
            const vec2i& center,
            float        radius,
            float        softness = 0.0f,
            float        inner    = -1.0f);

    void
    DrawLineFast(Image* image, const Rgbi& color, const vec2i& from, const vec2i& to);

    void
    DrawLineAntialiased(Image* image, const Rgb& color, const vec2f& from, const vec2f& to);

    void
    DrawLineAntialiased(Image* image, const Rgb& color, const vec2i& from, const vec2i& to);

    // position is lower left of text
    // each character is 8x8
    void
    DrawText(Image* image, const vec2i&       start_pos,
            const std::string& text,
            const Rgbi&        color,
            int                scale);

    void
    PasteImage(Image* image, const vec2i& position, const Image& source_image);


}  // namespace euphoria::core

#endif  // EUPHORIA_DRAW_H
