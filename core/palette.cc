#include "core/palette.h"

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
  Palette pal;
  for(int i = 0; i < count; i += 1)
  {
    float d = i / static_cast<float>(count - 1);
    const auto rgbf = rgb(Hsl{AngleTransform::Transform(from, d, to), saturation, lightness});
    pal.colors.push_back(rgbi(rgbf));
  }
  return pal;
}

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
    static const auto p = Palette{
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
    static const auto p = Palette{
      Color::White,
      Color::Silver,
      Color::Gray,
      Color::Black,
      Color::Navy,
      Color::Blue,
      Color::Aqua,
      Color::Teal,
      Color::Green,
      Color::Olive,
      Color::Lime,
      Color::Maroon,
      Color::Red,
      Color::Orange,
      Color::Yellow,
      Color::Purple,
      Color::Fuchsia,
      Color::AliceBlue,
      Color::AntiqueWhite,
      Color::AquaMarine,
      Color::Azure,
      Color::Beige,
      Color::Bisque,
      Color::BlanchedAlmond,
      Color::BlueViolet,
      Color::Brown,
      Color::BurlyWood,
      Color::CadetBlue,
      Color::Chartreuse,
      Color::Chocolate,
      Color::Coral,
      Color::CornflowerBlue,
      Color::Cornsilk,
      Color::Crimson,
      Color::Cyan,
      Color::Darkblue,
      Color::Darkcyan,
      Color::DarkGoldenRod,
      Color::Darkgray,
      Color::Darkgreen,
      Color::DarKkhaki,
      Color::DarkMagenta,
      Color::DarkOliveGreen,
      Color::DarkOrange,
      Color::DarkOrchid,
      Color::DarkRed,
      Color::DarkSalmon,
      Color::DarkSeaGreen,
      Color::DarkslateBlue,
      Color::DarkslateGray,
      Color::DarkTurquoise,
      Color::DarkBiolet,
      Color::DeeppInk,
      Color::DeepskyBlue,
      Color::DimGray,
      Color::DodgerBlue,
      Color::FireBrick,
      Color::FloralWhite,
      Color::ForestGreen,
      Color::Gainsboro,
      Color::GhostWhite,
      Color::Gold,
      Color::GoldenRod,
      Color::GreenYellow,
      Color::Honeydew,
      Color::Hotpink,
      Color::IndianRed,
      Color::Indigo,
      Color::Ivory,
      Color::Khaki,
      Color::Lavender,
      Color::LavenderBlush,
      Color::Lawngreen,
      Color::LemonChiffon,
      Color::LightBlue,
      Color::LightCoral,
      Color::LightCyan,
      Color::LightGoldenRodYellow,
      Color::LightGray,
      Color::LightGreen,
      Color::LightPink,
      Color::LightSalmon,
      Color::LightSeaGreen,
      Color::LightskyBlue,
      Color::LightslateGray,
      Color::LightSteelBlue,
      Color::LightYellow,
      Color::LimeGreen,
      Color::Linen,
      Color::Magenta,
      Color::MediumAquaMarine,
      Color::MediumBlue,
      Color::MediumOrchid,
      Color::MediumPurple,
      Color::MediumSeaGreen,
      Color::MediumslateBlue,
      Color::MediumSpringGreen,
      Color::MediumTurquoise,
      Color::MediumVioletRed,
      Color::MidnightBlue,
      Color::Mintcream,
      Color::Mistyrose,
      Color::Moccasin,
      Color::NavajoWhite,
      Color::Oldlace,
      Color::OliveDrab,
      Color::OrangeRed,
      Color::Orchid,
      Color::PaleGoldenRod,
      Color::PaleGreen,
      Color::PaleTurquoise,
      Color::PaleVioletRed,
      Color::Papayawhip,
      Color::Peachpuff,
      Color::Peru,
      Color::Pink,
      Color::Plum,
      Color::PowderBlue,
      Color::Rosybrown,
      Color::Royalblue,
      Color::SaddleBrown,
      Color::Salmon,
      Color::SandyBrown,
      Color::Seagreen,
      Color::Seashell,
      Color::Sienna,
      Color::SkyBlue,
      Color::SlateBlue,
      Color::SlateGray,
      Color::Snow,
      Color::SpringGreen,
      Color::SteelBlue,
      Color::Tan,
      Color::Thistle,
      Color::Tomato,
      Color::Turquoise,
      Color::Violet,
      Color::Wheat,
      Color::WhiteSmoke,
      Color::YellowGreen
    };
    return p;
  }
}  // namespace palette

