#ifndef EUPHORIA_DRAW_H
#define EUPHORIA_DRAW_H

#include "core/image.h"
#include "core/rgb.h"
#include "core/rect.h"
#include "core/vec2.h"

#include <string>

namespace euphoria::core
{
    struct Draw
    {
    public:
        explicit Draw(Image* image);

        Recti
        WholeImage() const;

        Draw&
        Clear(const Rgbi& color);

        Draw&
        Rect(const Rgbi& color, const Recti& rect);

        Draw&
        Square(const Rgbi& color, int x, int y, int size);

        Draw&
        Circle(const Rgb&   color,
               const vec2i& center,
               float        radius,
               float        softness = 0.0f,
               float        inner    = -1.0f);

        Draw&
        LineFast(const Rgbi& color, const vec2i& from, const vec2i& to);

        Draw&
        LineAntialiased(const Rgb& color, const vec2f& from, const vec2f& to);

        Draw&
        LineAntialiased(const Rgb& color, const vec2i& from, const vec2i& to);

        // position is lower left of text
        // each character is 8x8
        Draw&
        Text(const vec2i&       start_pos,
             const std::string& text,
             const Rgbi&        color,
             int                scale);

        Draw&
        PasteImage(const vec2i& position, const Image& source_image);

    private:
        Image* image_;
    };

}  // namespace euphoria::core

#endif  // EUPHORIA_DRAW_H
