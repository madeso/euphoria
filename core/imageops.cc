#include "core/imageops.h"

#include "core/image.h"
#include "core/palette.h"

Table<char> ImageToStringTable(const Image& img, const std::vector<ImageMapAction>& map)
{
  auto pal = Palette::Empty("");
  for(const auto m: map)
  {
    pal.colors.push_back(m.from_color);
  }

  auto ret = Table<char>::FromWidthHeight(img.GetWidth(), img.GetHeight(), ' ');
  ret.SetAll([&pal, &map, &img](int x, int y)
    {
      const auto p = img.GetPixel(x,y);
      const auto index = pal.GetIndexClosest(Rgbi{p.r, p.g, p.b});
      return map[index].to;
    }
  );

  return ret;
}

Table<char> ImageToStringTable(const Image& img, bool shorter)
{
  auto ret = Table<char>::FromWidthHeight(img.GetWidth(), img.GetHeight(), ' ');
  ret.SetAll([shorter, &img](int x, int y)
    {
      // http://paulbourke.net/dataformats/asciiart/
      const std::string characters = shorter ? 
        " .:-=+*#%@"
        :
        "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. "
        ;
      const auto p = 1.0f - rgb(img.GetPixel(x,y)).r;
      const auto index = Floori(p*(characters.size()-1));
      return characters[index];
    }
  );

  return ret;
}




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

