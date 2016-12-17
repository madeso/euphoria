#include "core/rgb.h"
#include <iostream>

Rgb::Rgb(const float red, const float green, const float blue)
{
  SetRgb(red, green, blue);
}

Rgb::Rgb(const float gray)
{
  SetRgb(gray, gray, gray);
}
Rgb::Rgb(const int all)
{
  SetRgb(all);
}

const float Rgb::GetRed() const
{
  return r;
}
const float Rgb::GetGreen() const
{
  return g;
}
const float Rgb::GetBlue() const
{
  return b;
}

/*const float* Rgb::data() const
{
  return mdata.array;
}*/

void Rgb::SetRed(const float v)
{
  r = v;
}
void Rgb::SetGreen(const float v)
{
  g = v;
}
void Rgb::SetBlue(const float v)
{
  b = v;
}

/*float& Rgb::operator[](int index)
{
  return mdata.array[index];
}
const float& Rgb::operator[](int index) const
{
  return mdata.array[index];
}*/

void Rgb::SetRgb(const float red, const float green, const float blue)
{
  r = red;
  g = green;
  b = blue;
}

namespace // local
{
const float GetComponent(int i, int steps)
{
  const int value = ((i >> 4 * steps) & 0xff);
  return value / 255.0f;
}
}
void Rgb::SetRgb(int rgb)
{
  const float b = GetComponent(rgb, 0);
  const float g = GetComponent(rgb, 1);
  const float r = GetComponent(rgb, 2);
  SetRgb(r, g, b);
}

const Rgb Rgb::From(Color color) {
  switch (color) {
    case Color::White:
      return Rgb(0xffffff);
    case Color::Silver:
      return Rgb(0xc0c0c0);
    case Color::Gray:
      return Rgb(0x808080);
    case Color::Black:
      return Rgb(0x000000);
    case Color::Navy:
      return Rgb(0x000080);
    case Color::Blue:
      return Rgb(0x0000ff);
    case Color::Aqua:
      return Rgb(0x00ffff);
    case Color::Teal:
      return Rgb(0x008080);
    case Color::Green:
      return Rgb(0x008000);
    case Color::Olive:
      return Rgb(0x808000);
    case Color::Lime:
      return Rgb(0x00ff00);
    case Color::Maroon:
      return Rgb(0x800000);
    case Color::Red:
      return Rgb(0xff0000);
    case Color::Orange:
      return Rgb(0xffa500);
    case Color::Yellow:
      return Rgb(0xffff00);
    case Color::Purple:
      return Rgb(0x800080);
    case Color::Fuchsia:
      return Rgb(0xff00ff);
    case Color::AliceBlue:
      return Rgb(0xf0f8ff);
    case Color::AntiqueWhite:
      return Rgb(0xfaebd7);
    case Color::AquaMarine:
      return Rgb(0x7fffd4);
    case Color::Azure:
      return Rgb(0xf0ffff);
    case Color::Beige:
      return Rgb(0xf5f5dc);
    case Color::Bisque:
      return Rgb(0xffe4c4);
    case Color::BlanchedAlmond:
      return Rgb(0xffebcd);
    case Color::BlueViolet:
      return Rgb(0x8a2be2);
    case Color::Brown:
      return Rgb(0xa52a2a);
    case Color::BurlyWood:
      return Rgb(0xdeb887);
    case Color::CadetBlue:
      return Rgb(0x5f9ea0);
    case Color::Chartreuse:
      return Rgb(0x7fff00);
    case Color::Chocolate:
      return Rgb(0xd2691e);
    case Color::Coral:
      return Rgb(0xff7f50);
    case Color::CornflowerBlue:
      return Rgb(0x6495ed);
    case Color::Cornsilk:
      return Rgb(0xfff8dc);
    case Color::Crimson:
      return Rgb(0xdc143c);
    case Color::Cyan:
      return Rgb(0x00ffff);
    case Color::Darkblue:
      return Rgb(0x00008b);
    case Color::Darkcyan:
      return Rgb(0x008b8b);
    case Color::DarkGoldenRod:
      return Rgb(0xb8860b);
    case Color::Darkgray:
      return Rgb(0xa9a9a9);
    case Color::Darkgreen:
      return Rgb(0x006400);
    case Color::DarKkhaki:
      return Rgb(0xbdb76b);
    case Color::DarkMagenta:
      return Rgb(0x8b008b);
    case Color::DarkOliveGreen:
      return Rgb(0x556b2f);
    case Color::DarkOrange:
      return Rgb(0xff8c00);
    case Color::DarkOrchid:
      return Rgb(0x9932cc);
    case Color::DarkRed:
      return Rgb(0x8b0000);
    case Color::DarkSalmon:
      return Rgb(0xe9967a);
    case Color::DarkSeaGreen:
      return Rgb(0x8fbc8f);
    case Color::DarkslateBlue:
      return Rgb(0x483d8b);
    case Color::DarkslateGray:
      return Rgb(0x2f4f4f);
    case Color::DarkTurquoise:
      return Rgb(0x00ced1);
    case Color::DarkBiolet:
      return Rgb(0x9400d3);
    case Color::DeeppInk:
      return Rgb(0xff1493);
    case Color::DeepskyBlue:
      return Rgb(0x00bfff);
    case Color::DimGray:
      return Rgb(0x696969);
    case Color::DodgerBlue:
      return Rgb(0x1e90ff);
    case Color::FireBrick:
      return Rgb(0xb22222);
    case Color::FloralWhite:
      return Rgb(0xfffaf0);
    case Color::ForestGreen:
      return Rgb(0x228b22);
    case Color::Gainsboro:
      return Rgb(0xdcdcdc);
    case Color::GhostWhite:
      return Rgb(0xf8f8ff);
    case Color::Gold:
      return Rgb(0xffd700);
    case Color::GoldenRod:
      return Rgb(0xdaa520);
    case Color::GreenYellow:
      return Rgb(0xadff2f);
    case Color::Honeydew:
      return Rgb(0xf0fff0);
    case Color::Hotpink:
      return Rgb(0xff69b4);
    case Color::IndianRed:
      return Rgb(0xcd5c5c);
    case Color::Indigo:
      return Rgb(0x4b0082);
    case Color::Ivory:
      return Rgb(0xfffff0);
    case Color::Khaki:
      return Rgb(0xf0e68c);
    case Color::Lavender:
      return Rgb(0xe6e6fa);
    case Color::LavenderBlush:
      return Rgb(0xfff0f5);
    case Color::Lawngreen:
      return Rgb(0x7cfc00);
    case Color::LemonChiffon:
      return Rgb(0xfffacd);
    case Color::LightBlue:
      return Rgb(0xadd8e6);
    case Color::LightCoral:
      return Rgb(0xf08080);
    case Color::LightCyan:
      return Rgb(0xe0ffff);
    case Color::LightGoldenRodYellow:
      return Rgb(0xfafad2);
    case Color::LightGray:
      return Rgb(0xd3d3d3);
    case Color::LightGreen:
      return Rgb(0x90ee90);
    case Color::LightPink:
      return Rgb(0xffb6c1);
    case Color::LightSalmon:
      return Rgb(0xffa07a);
    case Color::LightSeaGreen:
      return Rgb(0x20b2aa);
    case Color::LightskyBlue:
      return Rgb(0x87cefa);
    case Color::LightslateGray:
      return Rgb(0x778899);
    case Color::LightSteelBlue:
      return Rgb(0xb0c4de);
    case Color::LightYellow:
      return Rgb(0xffffe0);
    case Color::LimeGreen:
      return Rgb(0x32cd32);
    case Color::Linen:
      return Rgb(0xfaf0e6);
    case Color::Magenta:
      return Rgb(0xff00ff);
    case Color::MediumAquaMarine:
      return Rgb(0x66cdaa);
    case Color::MediumBlue:
      return Rgb(0x0000cd);
    case Color::MediumOrchid:
      return Rgb(0xba55d3);
    case Color::MediumPurple:
      return Rgb(0x9370db);
    case Color::MediumSeaGreen:
      return Rgb(0x3cb371);
    case Color::MediumslateBlue:
      return Rgb(0x7b68ee);
    case Color::MediumSpringGreen:
      return Rgb(0x00fa9a);
    case Color::MediumTurquoise:
      return Rgb(0x48d1cc);
    case Color::MediumVioletRed:
      return Rgb(0xc71585);
    case Color::MidnightBlue:
      return Rgb(0x191970);
    case Color::Mintcream:
      return Rgb(0xf5fffa);
    case Color::Mistyrose:
      return Rgb(0xffe4e1);
    case Color::Moccasin:
      return Rgb(0xffe4b5);
    case Color::NavajoWhite:
      return Rgb(0xffdead);
    case Color::Oldlace:
      return Rgb(0xfdf5e6);
    case Color::OliveDrab:
      return Rgb(0x6b8e23);
    case Color::OrangeRed:
      return Rgb(0xff4500);
    case Color::Orchid:
      return Rgb(0xda70d6);
    case Color::PaleGoldenRod:
      return Rgb(0xeee8aa);
    case Color::PaleGreen:
      return Rgb(0x98fb98);
    case Color::PaleTurquoise:
      return Rgb(0xafeeee);
    case Color::PaleVioletRed:
      return Rgb(0xdb7093);
    case Color::Papayawhip:
      return Rgb(0xffefd5);
    case Color::Peachpuff:
      return Rgb(0xffdab9);
    case Color::Peru:
      return Rgb(0xcd853f);
    case Color::Pink:
      return Rgb(0xffc0cb);
    case Color::Plum:
      return Rgb(0xdda0dd);
    case Color::PowderBlue:
      return Rgb(0xb0e0e6);
    case Color::Rosybrown:
      return Rgb(0xbc8f8f);
    case Color::Royalblue:
      return Rgb(0x4169e1);
    case Color::SaddleBrown:
      return Rgb(0x8b4513);
    case Color::Salmon:
      return Rgb(0xfa8072);
    case Color::SandyBrown:
      return Rgb(0xf4a460);
    case Color::Seagreen:
      return Rgb(0x2e8b57);
    case Color::Seashell:
      return Rgb(0xfff5ee);
    case Color::Sienna:
      return Rgb(0xa0522d);
    case Color::SkyBlue:
      return Rgb(0x87ceeb);
    case Color::SlateBlue:
      return Rgb(0x6a5acd);
    case Color::SlateGray:
      return Rgb(0x708090);
    case Color::Snow:
      return Rgb(0xfffafa);
    case Color::SpringGreen:
      return Rgb(0x00ff7f);
    case Color::SteelBlue:
      return Rgb(0x4682b4);
    case Color::Tan:
      return Rgb(0xd2b48c);
    case Color::Thistle:
      return Rgb(0xd8bfd8);
    case Color::Tomato:
      return Rgb(0xff6347);
    case Color::Turquoise:
      return Rgb(0x40e0d0);
    case Color::Violet:
      return Rgb(0xee82ee);
    case Color::Wheat:
      return Rgb(0xf5deb3);
    case Color::WhiteSmoke:
      return Rgb(0xf5f5f5);
    case Color::YellowGreen:
      return Rgb(0x9acd32);
    default:
      std::cerr << "invalid color name\n";
      return Rgb(0x9acd32);
  }
}

Rgba::Rgba(const float ar, const float ag, const float ab, const float aa)
    : r(ar), g(ag), b(ab), a(aa)
{
}

Rgba::Rgba(const Rgb& c, const float aa)
    : r(c.GetRed()), g(c.GetGreen()), b(c.GetBlue()), a(aa)
{
}

Rgba::Rgba(const float ar, const float ag, const float ab)
    : r(ar), g(ag), b(ab), a(1)
{
}

Rgba::Rgba(const float gray)
    : r(gray), g(gray), b(gray), a(1)
{
}

const float* Rgba::GetData() const
{
  return &r;
}

const float Rgba::GetRed() const
{
  return r;
}
const float Rgba::GetGreen() const
{
  return g;
}
const float Rgba::GetBlue() const
{
  return b;
}
const float Rgba::GetAlpha() const
{
  return a;
}

void Rgba::SetRed(const float v)
{
  r = v;
}
void Rgba::SetGreen(const float v)
{
  g = v;
}
void Rgba::SetBlue(const float v)
{
  b = v;
}
void Rgba::SetAlpha(const float v)
{
  a = v;
}

void Rgba::SetRgb(const float ar, const float ag, const float ab)
{
  SetRed(ar);
  SetGreen(ag);
  SetBlue(ab);
  SetAlpha(1);
}

