#include "core/palette.h"

#include "core/assert.h"

#include "core/numeric.h"
#include "core/random.h"
#include "core/colors.h"

namespace
{
  float
  Diff(const Rgbi& lhs, const Rgbi& rhs)
  {
    return Square(lhs.r - rhs.r) + Square(lhs.g - rhs.g) +
           Square(lhs.b - rhs.b);
  }
}  // namespace

Palette
Palette::Empty(const std::string& name)
{
  return Palette{name};
}

const Rgbi&
Palette::GetRandomColor(Random* r) const
{
  return r->Next(colors);
}

const Rgbi&
Palette::GetSafeIndex(unsigned int i) const
{
  return colors[i % colors.size()];
}

const Rgbi&
Palette::GetClosestColor(const Rgbi& c) const
{
  return colors[GetIndexClosest(c)];
}

unsigned int
Palette::GetIndexClosest(const Rgbi& c) const
{
  ASSERT(!colors.empty());
  auto diff_best = Diff(c, colors[0]);

  unsigned int index_best = 0;
  const auto   size       = colors.size();
  for(unsigned int index = 1; index < size; index += 1)
  {
    const auto diff = Diff(c, colors[index]);
    if(diff < diff_best)
    {
      diff_best  = diff;
      index_best = index;
    }
  }

  return index_best;
}

Palette
Palette::Rainbow(int count, float saturation, float lightness)
{
  return Rainbow(
      count,
      Angle::FromRadians(0),
      Angle::FromPercentOf360(
          Max(1.0f, (count - 1) / static_cast<float>(count))),
      saturation,
      lightness);
}

Palette
Palette::Rainbow(
    int          count,
    const Angle& from,
    const Angle& to,
    float        saturation,
    float        lightness)
{
  ASSERT(count > 1);
  auto pal = Palette::Empty("Rainbow");
  for(int i = 0; i < count; i += 1)
  {
    float d = i / static_cast<float>(count - 1);
    const auto rgbf = rgb(Hsl{AngleTransform::Transform(from, d, to), saturation, lightness});
    pal.colors.push_back(rgbi(rgbf));
  }
  return pal;
}

Palette::Palette(const std::string& n) : name(n) {}

////////////////////////////////////////////////////////////////////////////////

namespace
{
  Rgbi
  C(unsigned int hex)
  {
    return Rgbi::FromHex(hex);
  }
}  // namespace

namespace palette
{
  const Palette&
  Dawnbringer()
  {
    static const auto p = Palette{"dawnbringer",
        C(0x140C1C),
        C(0x442434),
        C(0x30346D),
        C(0x4E4A4E),
        C(0x854C30),
        C(0x346524),
        C(0xD04648),
        C(0x757161),
        C(0x597DCE),
        C(0xD27D2C),
        C(0x8595A1),
        C(0x6DAA2C),
        C(0xD2AA99),
        C(0x6DC2CA),
        C(0xDAD45E),
        C(0xDEEED6),
    };
    return p;
  }


  const Palette&
  NamedColors()
  {
    static const auto p = Palette{"named",
        Color::Black,
        Color::DarkGray,
        Color::Gray,
        Color::Red,
        Color::Blue,
        Color::Green,
        Color::Brown,
        Color::Purple,
        Color::LightGray,
        Color::LightGreen,
        Color::LightBlue,
        Color::Cyan,
        Color::Orange,
        Color::Yellow,
        Color::Tan,
        Color::Pink,
        Color::White,
        Color::Beige
      };
    return p;
  }
}  // namespace palette

