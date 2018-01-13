#ifndef CORE_RGB_H
#define CORE_RGB_H

#include <string>

#include "core/colors.h"

// unsigned char colors: 0 - 255
class Rgbi;
class Rgbai;

// float based colors: 0.0 - 1.0
class Rgb;
class Rgba;

////////////////////////////////////////////////////////////////////////////////
// Rgb no alpha support - int based
class Rgbi
{
 public:
  Rgbi(unsigned char red, unsigned char green, unsigned char blue);
  explicit Rgbi(unsigned char gray);
  explicit Rgbi(const Rgbai& rgb);
  Rgbi(Color color);
  Rgbi(DawnbringerPalette color);

  explicit Rgbi(const Rgb& rgb);

  void
  SetFromHexColor(int hex);

  unsigned char r;
  unsigned char g;
  unsigned char b;
};

////////////////////////////////////////////////////////////////////////////////
// Rgb with alpha - int based

class Rgbai
{
 public:
  Rgbai(const Rgbi& rgb, unsigned char alpha = 1.0f);

  explicit Rgbai(const Rgba& rgba);

  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};

////////////////////////////////////////////////////////////////////////////////
// Rgb no alpha support - float based

class Rgb
{
 public:
  Rgb(const float red, const float green, const float blue);
  explicit Rgb(const float gray);
  explicit Rgb(const Rgba& rgb);
  explicit Rgb(const Rgbai& rgb);
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
  Rgba(const Rgb& rgb, float alpha = 1.0f);

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
  const unsigned char
  GetComponent(unsigned int i, int steps);

  const unsigned char
  GetRed(unsigned int rgb);

  const unsigned char
  GetGreen(unsigned int rgb);

  const unsigned char
  GetBlue(unsigned int rgb);

  float
  ToFloat(unsigned char c);

  unsigned char
  ToUnsignedChar(float f);
}

#endif  // CORE_RGB_H
