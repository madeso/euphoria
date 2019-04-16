#ifndef EUPHORIA_DRAW_H
#define EUPHORIA_DRAW_H

#include "core/image.h"
#include "core/rgb.h"
#include "core/rect.h"
#include "core/vec2.h"

#include <string>

class Draw
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
  Circle(
      const Rgb&     color,
      const point2i& center,
      float          radius,
      float          softness = 0.0f,
      float          inner    = -1.0f);

  Draw&
  LineFast(const Rgbi& color, const point2i& from, const point2i& to);

  Draw&
  LineAntialiased(const Rgb& color, const point2f& from, const point2f& to);

  Draw&
  LineAntialiased(const Rgb& color, const point2i& from, const point2i& to);

  // position is lower left of text
  // each character is 8x8
  Draw&
  Text(
      const point2i&     start_pos,
      const std::string& text,
      const Rgbi&        color,
      int                scale);

  Draw&
  PasteImage(const point2i& position, const Image& source_image);

 private:
  Image* image_;
};


#endif  // EUPHORIA_DRAW_H
