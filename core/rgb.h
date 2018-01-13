#ifndef CORE_RGB_H
#define CORE_RGB_H

#include <string>

#include "core/colors.h"

// float based colors 0-1
class Rgb;
class Rgba;


////////////////////////////////////////////////////////////////////////////////
// Rgb no alpha support - float based

class Rgb
{
 public:
  Rgb(const float red, const float green, const float blue);
  explicit Rgb(const float gray);
  explicit Rgb(const Rgba& rgb);
  Rgb(Color color);
  Rgb(DawnbringerPalette color);

  void
  SetFromHexColor(int hex);

  float r;
  float g;
  float b;
};


////////////////////////////////////////////////////////////////////////////////
// Rgb with alpha - float based

class Rgba
{
 public:
  Rgba(const Rgb& rgb, const float alpha = 1.0f);

  float r;
  float g;
  float b;
  float a;
};


////////////////////////////////////////////////////////////////////////////////
// For printing

std::ostream&
operator<<(std::ostream& stream, const Rgb& v);

std::ostream&
operator<<(std::ostream& stream, const Rgba& v);


////////////////////////////////////////////////////////////////////////////////
// Transforms

struct RgbTransform
{
  static Rgb
  Transform(const Rgb& from, float v, const Rgb to);
};


////////////////////////////////////////////////////////////////////////////////
// Internal utility

namespace colorutil
{
  // internal function, exposed for unit tests
  const int
  GetComponent(unsigned int i, int steps);

  const int
  GetRed(unsigned int rgb);

  const int
  GetGreen(unsigned int rgb);

  const int
  GetBlue(unsigned int rgb);
}

#endif  // CORE_RGB_H
