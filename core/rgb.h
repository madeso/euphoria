#ifndef CORE_RGB_H
#define CORE_RGB_H

#include <string>

#include "core/colors.h"
#include "core/angle.h"

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
  Rgbi(Color color);

  explicit Rgbi(const Rgb& rgb);

  static Rgbi
  FromHex(unsigned int hex);

  unsigned char r;
  unsigned char g;
  unsigned char b;
};

////////////////////////////////////////////////////////////////////////////////
// Rgb with alpha - int based

class Rgbai
{
 public:
  Rgbai(const Rgbi& rgb, unsigned char alpha = 255);

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
  Rgb(float red, float green, float blue);
  explicit Rgb(float gray);

  Rgb(Color color);

  static Rgb
  FromHex(unsigned int hex);

  float r;
  float g;
  float b;
};

float dot(const Rgb& lhs, const Rgb& rhs);
Rgb Clamp(const Rgb& r);


////////////////////////////////////////////////////////////////////////////////
// HSL

struct Hsl
{
  // hue, saturation, lightness
  Angle h;
  float s;  // 0-1
  float l;  // 0-1

  static const Angle& Red ();
  static const Angle& Yellow ();
  static const Angle& Green ();
  static const Angle& Cyan ();
  static const Angle& Blue ();
  static const Angle& Magenta ();
};

enum class Method
{
  Absolute,
  Relative
};

////////////////////////////////////////////////////////////////////////////////
// Util functions

Hsl
saturate(const Hsl& ahsl, float amount, Method method);

Hsl
desaturate(const Hsl& ahsl, float amount, Method method);

Hsl
lighten(const Hsl& ahsl, float amount, Method method);

Hsl
darken(const Hsl& ahsl, float amount, Method method);

/** Makes a color brighter or darker.
 * The +1 makes it white, 0% is no change, -1 makes it black.
 */
Rgb
ShadeColor(const Rgb& rgb, float percentage);


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
operator<<(std::ostream& stream, const Rgbi& v);

std::ostream&
operator<<(std::ostream& stream, const Rgbai& v);

std::ostream&
operator<<(std::ostream& stream, const Rgb& v);

std::ostream&
operator<<(std::ostream& stream, const Rgba& v);

std::ostream&
operator<<(std::ostream& stream, const Hsl& v);

////////////////////////////////////////////////////////////////////////////////
// Default compare

bool
operator==(const Rgbai& lhs, const Rgbai& rhs);

////////////////////////////////////////////////////////////////////////////////
// Convert functions (rgb)

Rgb
rgb(const Rgbi& rgb);

Rgb
rgb(const Rgba& rgb);

Rgb
rgb(const Rgbai& rgb);

Rgb
rgb(const Hsl& c);

// Convert functions (hsl)

Hsl
hsl(const Rgb& c);

// Convert functions (rgbi)

Rgbi
rgbi(const Rgb& rgb);

Rgbi
rgbi(const Rgba& rgb);

Rgbi
rgbi(const Rgbai& rgb);

////////////////////////////////////////////////////////////////////////////////
// Transforms

struct RgbTransform
{
  static Rgb
  Transform(const Rgb& from, float v, const Rgb& to);
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

  unsigned int
  FromStringToHex(const std::string& str);
}

#endif  // CORE_RGB_H
