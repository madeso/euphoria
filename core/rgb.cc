#include "core/rgb.h"
#include "core/interpolate.h"
#include "core/numeric.h"
#include <iostream>
#include <map>

////////////////////////////////////////////////////////////////////////////////

Rgbi::Rgbi(unsigned char red, unsigned char green, unsigned char blue)
    : r(red)
    , g(green)
    , b(blue)
{
}

Rgbi::Rgbi(unsigned char gray)
    : r(gray)
    , g(gray)
    , b(gray)
{
}
Rgbi::Rgbi(const Rgbai& rgb)
    : r(rgb.r)
    , g(rgb.g)
    , b(rgb.b)
{
}

Rgbi::Rgbi(Color color)
{
  SetFromHexColor(ToColorHex(color));
}

Rgbi::Rgbi(DawnbringerPalette color)
{
  SetFromHexColor(ToColorHex(color));
}

Rgbi::Rgbi(const Rgb& rgb)
    : r(colorutil::ToUnsignedChar(rgb.r))
    , g(colorutil::ToUnsignedChar(rgb.g))
    , b(colorutil::ToUnsignedChar(rgb.b))
{
}

void
Rgbi::SetFromHexColor(int hex)
{
  b = colorutil::GetBlue(hex);
  g = colorutil::GetGreen(hex);
  r = colorutil::GetRed(hex);
}


////////////////////////////////////////////////////////////////////////////////

Rgbai::Rgbai(const Rgbi& rgb, unsigned char alpha)
    : r(rgb.r)
    , g(rgb.g)
    , b(rgb.b)
    , a(alpha)
{
}

Rgbai::Rgbai(const Rgba& rgba)
    : r(colorutil::ToUnsignedChar(rgba.r))
    , g(colorutil::ToUnsignedChar(rgba.g))
    , b(colorutil::ToUnsignedChar(rgba.b))
    , a(colorutil::ToUnsignedChar(rgba.a))
{
}

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
Rgb::Rgb(const Rgba& rgb)
    : r(rgb.r)
    , g(rgb.g)
    , b(rgb.b)
{
}

Rgb::Rgb(const Rgbai& rgb)
    : r(colorutil::ToFloat(rgb.r))
    , g(colorutil::ToFloat(rgb.g))
    , b(colorutil::ToFloat(rgb.b))
{
}

Rgb::Rgb(Color color)
{
  SetFromHexColor(ToColorHex(color));
}

Rgb::Rgb(DawnbringerPalette color)
{
  SetFromHexColor(ToColorHex(color));
}

void
Rgb::SetFromHexColor(int hex)
{
  b = colorutil::ToFloat(colorutil::GetBlue(hex));
  g = colorutil::ToFloat(colorutil::GetGreen(hex));
  r = colorutil::ToFloat(colorutil::GetRed(hex));
}


////////////////////////////////////////////////////////////////////////////////

Rgba::Rgba(const Rgb& rgb, float alpha)
    : r(rgb.r)
    , g(rgb.g)
    , b(rgb.b)
    , a(alpha)
{
}

////////////////////////////////////////////////////////////////////////////////

std::ostream&
operator<<(std::ostream& stream, const Rgb& v)
{
  return stream << "(" << v.r << ", " << v.g << ", " << v.b << ")";
}


std::ostream&
operator<<(std::ostream& stream, const Rgba& v)
{
  return stream << "(" << v.r << ", " << v.g << ", " << v.b << ", " << v.a
                << ")";
}

////////////////////////////////////////////////////////////////////////////////

Rgb
RgbTransform::Transform(const Rgb& from, float v, const Rgb to)
{
  return Rgb(
      FloatTransform::Transform(from.r, v, to.r),
      FloatTransform::Transform(from.g, v, to.g),
      FloatTransform::Transform(from.b, v, to.b));
}

////////////////////////////////////////////////////////////////////////////////

namespace colorutil
{
  const unsigned char
  GetComponent(unsigned int i, int steps)
  {
    const int value = ((i >> 8 * steps) & 0xff);
    ASSERTX(IsWithinInclusivei(0, value, 255), value);
    return static_cast<unsigned char>(value);
  }

  const unsigned char
  GetRed(unsigned int rgb)
  {
    return GetComponent(rgb, 2);
  }

  const unsigned char
  GetGreen(unsigned int rgb)
  {
    return GetComponent(rgb, 1);
  }

  const unsigned char
  GetBlue(unsigned int rgb)
  {
    return GetComponent(rgb, 0);
  }

  float
  ToFloat(unsigned char c)
  {
    return c / 255.0f;
  }

  unsigned char
  ToUnsignedChar(float f)
  {
    return static_cast<unsigned char>(f * 255.0f);
  }
}  // namespace colorutil
