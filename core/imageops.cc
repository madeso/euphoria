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
