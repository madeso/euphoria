#include "core/imagefilters.h"

#include "core/image.h"
#include "core/palette.h"
#include "core/table.h"
#include "core/vec3.h"


namespace
{
  Rgbai Gray(unsigned char g, unsigned char a)
  {
    return {Rgbi{g, g, g}, a};
  }
}

// 
// https://twitter.com/FreyaHolmer/status/1116502994684530688
void MakeGrayscale(Image* image, Grayscale grayscale)
{
  switch(grayscale)
  {
    case Grayscale::R: image->Filter([](const Rgbai& c)
            {
              return Gray(c.r, c.a);
            }
          );
      break;
    case Grayscale::G: image->Filter([](const Rgbai& c)
            {
              return Gray(c.g, c.a);
            }
          );
      break;
    case Grayscale::B: image->Filter([](const Rgbai& c)
            {
              return Gray(c.b, c.a);
            }
          );
      break;
    case Grayscale::Max: image->Filter([](const Rgbai& c)
            {
              return Gray(Max(c.r, Max(c.g, c.b)), c.a);
            }
          );
      break;
    case Grayscale::Gamma: image->Filter([](const Rgbai& c)
            {
              const auto d = dot(rgb(c), Rgb(0.22f, 0.707f, 0.071f));
              return Rgbai(rgbi(Rgb(d)), c.a);
            }
          );
      break;
    case Grayscale::Linear: image->Filter([](const Rgbai& c)
            {
              const auto d = dot(rgb(c), Rgb(0.0397f, 0.4580f, 0.0061f));
              return Rgbai(rgbi(Rgb(d)), c.a);
            }
          );
      break;
    case Grayscale::Average: image->Filter([](const Rgbai& c)
            {
              const auto cc = rgb(c);
              const auto g = (cc.r + cc.g + cc.b)/3;
              return Rgbai(rgbi(Rgb(g)), c.a);
            }
          );
      break;
  }
}

void MatchPalette(Image* image, const Palette& palette)
{
  image->Filter([&palette](const Rgbai& c) {
      const auto cc = rgbi(c);
      const auto nc = palette.GetClosestColor(cc);

      return Rgbai(nc, c.a);
      });
}

template<typename C>
Image NewImageFrom(const Image& image, C callback)
{
  Image ret;
  if(image.HasAlpha()) { ret.SetupWithAlphaSupport(image.GetWidth(), image.GetHeight(), -1); }
  else { ret.SetupNoAlphaSupport(image.GetWidth(), image.GetHeight(), -1); }
  ret.SetAllTopBottom(callback);
  return ret;
}

Image MatchPaletteDither(const Image& image, const Palette& palette)
{
  struct Error { float r=0; float g=0; float b=0; };
  auto errors = Table<Error>::FromWidthHeight(image.GetWidth(), image.GetHeight());
  const auto errors_range = errors.Indices();
  return NewImageFrom(image, [&](int x, int y) {
      auto pixel = image.GetPixel(x,y);
      auto new_color = rgb(pixel);
      const auto pixel_error = errors.Value(x,y);
      new_color.r += pixel_error.r;
      new_color.g += pixel_error.g;
      new_color.b += pixel_error.b;
      new_color = Clamp(new_color);
      const auto palette_color = palette.GetClosestColor(rgbi(new_color));

      const auto pcf = rgb(palette_color);
      const auto error = Error{new_color.r - pcf.r,
                               new_color.g - pcf.g,
                               new_color.b - pcf.b};
      const auto floyd_steinberg = std::vector<std::pair<vec2i, float>> {
        {vec2i( 1,  0), 7.0f/16.0f},
        {vec2i( 1, -1), 1.0f/16.0f},
        {vec2i( 0, -1), 5.0f/16.0f},
        {vec2i(-1, -1), 3.0f/16.0f}
      };

      for(auto fs: floyd_steinberg)
      {
        auto nx = static_cast<int>(x) + fs.first.x;
        auto ny = static_cast<int>(y) + fs.first.y;
        auto factor = fs.second;
        if(errors_range.ContainsInclusive(nx, ny))
        {
          auto& e = errors.RefValue(nx, ny);
          e.r += factor * error.r;
          e.g += factor * error.g;
          e.b += factor * error.b;
        }
      }

      return Rgbai(palette_color, pixel.a);
  });
}

vec3f Cvec3(const Rgbai c)
{
  const auto f = rgb(c);
  return {f.r, f.g, f.b};
}

Image EdgeDetection(const Image& image, float r)
{
  return NewImageFrom(image, [&](int x, int y) {
      const auto pixel = Cvec3(image.GetPixel(x,y));
      const auto top =  y == image.GetHeight() - 1 ? false : (pixel - Cvec3(image.GetPixel(x,   y+1))).GetLength() >= r;
      const auto left = x == 0                     ? false : (pixel - Cvec3(image.GetPixel(x-1, y  ))).GetLength() >= r;
      const bool edge = top || left;
      const auto c = edge ? Color::White : Color::Black;
      return Rgbai(c, 255);
  });
}


