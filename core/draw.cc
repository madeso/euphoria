#include "draw.h"

#include <utility>

#include "core/assert.h"

Draw::Draw(Image* image)
: image_(image)
{
  Assert(image);
}

Recti Draw::WholeImage() const
{
  return Recti::FromTopLeftWidthHeight(0, 0, image_->GetWidth(), image_->GetHeight());
}

Draw& Draw::Clear(const Rgb &color)
{
  return Square(color, WholeImage());
}

Draw& Draw::Square(const Rgb& color, const Recti& rect)
{
  const int left = rect.TopLeft().x;
  const int right = rect.TopRight().x;
  const int top = rect.TopLeft().y;
  const int bottom = rect.BottomLeft().y;
  for(int y=top; y<bottom; ++y) {
    for(int x=left; x<right; ++x) {
      image_->SetPixel(x, y, color);
    }
  }

  return *this;
}

Draw& Draw::Circle(const Rgb& color, const vec2i& center, float radius, float softness, float inner)
{
  const int left =  Max(0, floor(center.x - radius - softness));
  const int right = Min(image_->GetWidth(), ceil(center.x + radius + softness));
  const int top = Max(0, floor(center.y - radius - softness));
  const int bottom = Min(image_->GetWidth(), ceil(center.x + radius + softness));

  // color modes
  // nothing INNER-SOFTNESS fade INNER full RADIUS fade RADIUS+SOFTNESS nothing

  for(int y=top; y<bottom; ++y) {
    for(int x=left; x<right; ++x) {
      // todo: use length squared!
      const float sq = vec2f::FromTo(vec2f{static_cast<float>(x),static_cast<float>(y)},
                                     vec2f{static_cast<float>(center.x), static_cast<float>(center.y)}).GetLength();
      bool blend = false;
      float blend_factor = 1.0f;

      if( IsWithinInclusive(inner-softness, sq, inner) ) {
        blend_factor = To01(inner-softness, sq, inner);
        blend = true;
      }
      else if (IsWithinInclusive(radius, sq, radius+softness)) {
        blend_factor = 1.0f - To01(radius, sq, radius+softness);
        blend = true;
      }
      else if(IsWithinInclusive(inner, sq, radius)) {
        // full color
      }
      else {
        // outside
        continue;
      }

      const Rgb paint_color =
          blend
          ? RgbTransform::Transform(Rgb{image_->GetPixel(x,y)}, blend_factor, color)
          : color;

      image_->SetPixel(x, y, paint_color);
    }
  }

  return *this;
}

Draw& Draw::LineFast(const Rgb &color, const vec2i &from, const vec2i &to)
{
  // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

  const int x0 = from.x;
  const int y0 = from.y;
  const int x1 = to.x;
  const int y1 = to.y;

  int deltax = x1 - x0;
  int deltay = y1 - y0;
  const float deltaerr = deltax == 0 ? -1 : abs(deltay / static_cast<float>(deltax));
  float error = deltaerr - 0.5;
  int y = y0;
  for(int x=x0; x<x1; x+=1) {
    const bool valid_x = IsWithinInclusivei(0, x, image_->GetWidth()-1);
    const bool valid_y = IsWithinInclusivei(0, y, image_->GetHeight()-1);
    if(valid_x && valid_y) {
      image_->SetPixel(x, y, color);
    }

    if(deltaerr > 0) {
      error = error + deltaerr;
      if(error >= 0.5) {
        if(deltay>0) {
          y = y + 1;
        }
        else {
          y = y - 1;
        }
      }
      error = error - 1.0;
    }
  }

  return *this;
}

int ipart(float x) {
  return floor(x);
}

int round(float x) {
  return ipart(x + 0.5f);
}

// fractional part of x
float fpart(float x) {
  return x - floor(x);
}

float rfpart(float x) {
  return 1 - fpart(x);
}

void plot(int x, int y, float brightness, const Rgb& color, Image* image) {
  const bool valid_x = IsWithinInclusivei(0, x, image->GetWidth()-1);
  const bool valid_y = IsWithinInclusivei(0, y, image->GetHeight()-1);
  if(valid_x && valid_y) {
    const Rgb paint_color = RgbTransform::Transform(Rgb{image->GetPixel(x,y)}, brightness, color);
    image->SetPixel(x, y, paint_color);
  }
}

Draw& Draw::LineAntialiased(const Rgb& color, const vec2i& from, const vec2i& to) {
  return LineAntialiased(color, from.StaticCast<float>(), to.StaticCast<float>());
}

Draw& Draw::LineAntialiased(const Rgb& color, const vec2f& from, const vec2f& to) {
  // https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm
  float x0 = from.x;
  float y0 = from.y;
  float x1 = to.x;
  float y1 = to.y;


  bool steep = Abs(y1 - y0) > Abs(x1 - x0);

  if(steep) {
    std::swap(x0, y0);
    std::swap(x1, y1);
  }
  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  float dx = x1 - x0;
  float dy = y1 - y0;
  float gradient = dx == 0.0 ? 1.0 : dy / dx;

    // handle first endpoint
  int xend = round(x0);
  float yend = y0 + gradient * (xend - x0);
  float xgap = rfpart(x0 + 0.5);
  int xpxl1 = xend; // this will be used in the main loop
  float ypxl1 = ipart(yend);
  if(steep) {
    plot(ypxl1,   xpxl1, rfpart(yend) * xgap, color, image_);
    plot(ypxl1+1, xpxl1,  fpart(yend) * xgap, color, image_);
  }
  else {
    plot(xpxl1, ypxl1  , rfpart(yend) * xgap, color, image_);
    plot(xpxl1, ypxl1+1,  fpart(yend) * xgap, color, image_);
  }
  int intery = yend + gradient; // first y-intersection for the main loop

  // handle second endpoint
  xend = round(x1);
  yend = y1 + gradient * (xend - x1);
  xgap = fpart(x1 + 0.5);
  int xpxl2 = xend; //this will be used in the main loop
  int ypxl2 = ipart(yend);
  if(steep) {
    plot(ypxl2  , xpxl2, rfpart(yend) * xgap, color, image_);
    plot(ypxl2+1, xpxl2,  fpart(yend) * xgap, color, image_);
  }
  else {
    plot(xpxl2, ypxl2,  rfpart(yend) * xgap, color, image_);
    plot(xpxl2, ypxl2+1, fpart(yend) * xgap, color, image_);
  }

  // main loop
  if(steep) {
    for(int x=xpxl1 + 1; x<xpxl2 - 1; x+=1)
    {
      plot(ipart(intery), x, rfpart(intery), color, image_);
      plot(ipart(intery) + 1, x, fpart(intery), color, image_);
      intery = intery + gradient;
    }
  }
  else {
    for(int x = xpxl1 + 1; x< xpxl2 - 1; x+=1) {
      plot(x, ipart(intery),  rfpart(intery), color, image_);
      plot(x, ipart(intery)+1, fpart(intery), color, image_);
      intery = intery + gradient;
     }
   }
  return *this;
}
