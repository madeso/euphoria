#ifndef EUPHORIA_DRAW_H
#define EUPHORIA_DRAW_H

#include "core/image.h"
#include "core/rgb.h"
#include "core/rect.h"

#include <string>

class Draw {
 public:
  Draw(Image* image);

  Recti WholeImage() const;

  Draw& Clear(const Rgb& color);

  Draw& Square(const Rgb& color, const Recti& rect);
  Draw& Circle(const Rgb& color, const vec2i& center, float radius, float softness=0.0f, float inner=-1.0f);

  Draw& LineFast(const Rgb& color, const vec2i& from, const vec2i& to);
  Draw& LineAntialiased(const Rgb& color, const vec2f& from, const vec2f& to);
  Draw& LineAntialiased(const Rgb& color, const vec2i& from, const vec2i& to);

  Draw& Text(const vec2i& pos, const std::string& text, const Rgb& color, int scale);

 private:
  Image* image_;
};


#endif //EUPHORIA_DRAW_H
