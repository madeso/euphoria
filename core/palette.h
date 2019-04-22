#ifndef EUPHORIA_PALETTE_H
#define EUPHORIA_PALETTE_H

#include <string>

#include "core/rgb.h"


class Random;

struct Palette
{
  std::string name;
  std::vector<Rgbi> colors;

  template <typename... R>
  Palette(const std::string& n, R... c)
      : name(n)
      , colors{c...}
  {
  }

  static Palette Empty(const std::string& name);

  const Rgbi&
  GetRandomColor(Random* r) const;

  const Rgbi&
  GetSafeIndex(unsigned int i) const;

  const Rgbi&
  GetClosestColor(const Rgbi& c) const;

  unsigned int
  GetIndexClosest(const Rgbi& c) const;

  // rainbow functions based on the r documentation https://rdrr.io/r/grDevices/palettes.html

  static Palette
  Rainbow(int count, float saturation = 1, float lightness = 1);
  
  static Palette
  Rainbow(int count, const Angle& from, const Angle& to, float saturation, float lightness);

  private:
  explicit Palette(const std::string& n);
};

namespace palette
{
  // http://pixeljoint.com/forum/forum_posts.asp?TID=12795
  const Palette&
  Dawnbringer();

  const Palette&
  NamedColors();
}

#endif  // EUPHORIA_PALETTE_H
