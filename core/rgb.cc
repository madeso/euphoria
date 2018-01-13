#include "core/rgb.h"
#include "interpolate.h"
#include <iostream>
#include <map>

////////////////////////////////////////////////////////////////////////////////

Rgb::Rgb(const float red, const float green, const float blue)
    : r(0)
    , g(0)
    , b(0)
{
  SetRgb(red, green, blue);
}

Rgb::Rgb(const float gray)
    : r(0)
    , g(0)
    , b(0)
{
  SetRgb(gray, gray, gray);
}

Rgb::Rgb(const int rgb)
    : r(0)
    , g(0)
    , b(0)
{
  SetRgb(rgb);
}

Rgb::Rgb(const Rgba& rgb)
    : r(0)
    , g(0)
    , b(0)
{
  SetRgb(rgb.GetRed(), rgb.GetGreen(), rgb.GetBlue());
}


const float
Rgb::GetRed() const
{
  return r;
}
const float
Rgb::GetGreen() const
{
  return g;
}
const float
Rgb::GetBlue() const
{
  return b;
}

float*
Rgb::GetData()
{
  return &r;
}

const float* const
Rgb::GetData() const
{
  return &r;
}

void
Rgb::SetRed(const float v)
{
  r = v;
}
void
Rgb::SetGreen(const float v)
{
  g = v;
}
void
Rgb::SetBlue(const float v)
{
  b = v;
}

void
Rgb::SetRgb(const float red, const float green, const float blue)
{
  r = red;
  g = green;
  b = blue;
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
  const auto b = colorutil::GetBlue(rgb);
  const auto g = colorutil::GetGreen(rgb);
  const auto r = colorutil::GetRed(rgb);
  SetRgb(r / 255.0f, g / 255.0f, b / 255.0f);
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
  return stream << "(" << v.GetRed() << ", " << v.GetGreen() << ", "
                << v.GetBlue() << ")";
}

Rgb
RgbTransform::Transform(const Rgb& from, float v, const Rgb to)
{
  return Rgb(
      FloatTransform::Transform(from.GetRed(), v, to.GetRed()),
      FloatTransform::Transform(from.GetGreen(), v, to.GetGreen()),
      FloatTransform::Transform(from.GetBlue(), v, to.GetBlue()));
}

Rgba::Rgba(const Rgb& rgb, const float alpha)
    : r(rgb.GetRed())
    , g(rgb.GetGreen())
    , b(rgb.GetBlue())
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

const float*
Rgba::GetData() const
{
  return &r;
}

const float
Rgba::GetRed() const
{
  return r;
}
const float
Rgba::GetGreen() const
{
  return g;
}
const float
Rgba::GetBlue() const
{
  return b;
}
const float
Rgba::GetAlpha() const
{
  return a;
}

void
Rgba::SetRed(const float v)
{
  r = v;
}
void
Rgba::SetGreen(const float v)
{
  g = v;
}
void
Rgba::SetBlue(const float v)
{
  b = v;
}
void
Rgba::SetAlpha(const float v)
{
  a = v;
}

void
Rgba::SetRgb(const float red, const float green, const float blue)
{
  SetRed(red);
  SetGreen(green);
  SetBlue(blue);
  SetAlpha(1);
}

std::ostream&
operator<<(std::ostream& stream, const Rgba& v)
{
  return stream << "(" << v.GetRed() << ", " << v.GetGreen() << ", "
                << v.GetBlue() << ", " << v.GetAlpha() << ")";
}
