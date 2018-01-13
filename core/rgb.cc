#include "core/rgb.h"
#include "interpolate.h"
#include <iostream>
#include <map>

////////////////////////////////////////////////////////////////////////////////

Rgb::Rgb(const float red, const float green, const float blue)
    : r(red)
    , g(green)
    , b(blue)
{
}

Rgb::Rgb(const float gray)
    : r(gray)
    , g(gray)
    , b(gray)
{
}

Rgb::Rgb(const int rgb)
    : r(0)
    , g(0)
    , b(0)
{
  SetRgb(rgb);
}

Rgb::Rgb(const Rgba& rgb)
    : r(rgb.r)
    , g(rgb.g)
    , b(rgb.b)
{
}

namespace colorutil
{
  const int
  GetComponent(unsigned int i, int steps)
  {
    const int value = ((i >> 8 * steps) & 0xff);
    return value;
  }

  const int
  GetRed(unsigned int rgb)
  {
    return GetComponent(rgb, 2);
  }

  const int
  GetGreen(unsigned int rgb)
  {
    return GetComponent(rgb, 1);
  }

  const int
  GetBlue(unsigned int rgb)
  {
    return GetComponent(rgb, 0);
  }
}  // namespace colorutil

void
Rgb::SetRgb(int rgb)
{
  const auto blue  = colorutil::GetBlue(rgb);
  const auto green = colorutil::GetGreen(rgb);
  const auto red   = colorutil::GetRed(rgb);
  r                = red / 255.0f;
  g                = green / 255.0f;
  b                = blue / 255.0f;
}

const Rgb
Rgb::From(Color color)
{
  return Rgb(ToColorHex(color));
}

const Rgb
Rgb::From(DawnbringerPalette color)
{
  return Rgb(ToColorHex(color));
}

std::ostream&
operator<<(std::ostream& stream, const Rgb& v)
{
  return stream << "(" << v.r << ", " << v.g << ", " << v.b << ")";
}

Rgb
RgbTransform::Transform(const Rgb& from, float v, const Rgb to)
{
  return Rgb(
      FloatTransform::Transform(from.r, v, to.r),
      FloatTransform::Transform(from.g, v, to.g),
      FloatTransform::Transform(from.b, v, to.b));
}

Rgba::Rgba(const Rgb& rgb, const float alpha)
    : r(rgb.r)
    , g(rgb.g)
    , b(rgb.b)
    , a(alpha)
{
}

Rgba::Rgba(
    const float red, const float green, const float blue, const float alpha)
    : r(red)
    , g(green)
    , b(blue)
    , a(alpha)
{
}

Rgba::Rgba(const float gray, const float alpha)
    : r(gray)
    , g(gray)
    , b(gray)
    , a(alpha)
{
}

std::ostream&
operator<<(std::ostream& stream, const Rgba& v)
{
  return stream << "(" << v.r << ", " << v.g << ", " << v.b << ", " << v.a
                << ")";
}
