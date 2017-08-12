#ifndef CORE_RGB_H
#define CORE_RGB_H

// from http://www.uize.com/examples/sortable-color-table.html
enum class Color
{
  White,
  Silver,
  Gray,
  Black,
  Navy,
  Blue,
  Aqua,
  Teal,
  Green,
  Olive,
  Lime,
  Maroon,
  Red,
  Orange,
  Yellow,
  Purple,
  Fuchsia,
  AliceBlue,
  AntiqueWhite,
  AquaMarine,
  Azure,
  Beige,
  Bisque,
  BlanchedAlmond,
  BlueViolet,
  Brown,
  BurlyWood,
  CadetBlue,
  Chartreuse,
  Chocolate,
  Coral,
  CornflowerBlue,
  Cornsilk,
  Crimson,
  Cyan,
  Darkblue,
  Darkcyan,
  DarkGoldenRod,
  Darkgray,
  Darkgreen,
  DarKkhaki,
  DarkMagenta,
  DarkOliveGreen,
  DarkOrange,
  DarkOrchid,
  DarkRed,
  DarkSalmon,
  DarkSeaGreen,
  DarkslateBlue,
  DarkslateGray,
  DarkTurquoise,
  DarkBiolet,
  DeeppInk,
  DeepskyBlue,
  DimGray,
  DodgerBlue,
  FireBrick,
  FloralWhite,
  ForestGreen,
  Gainsboro,
  GhostWhite,
  Gold,
  GoldenRod,
  GreenYellow,
  Honeydew,
  Hotpink,
  IndianRed,
  Indigo,
  Ivory,
  Khaki,
  Lavender,
  LavenderBlush,
  Lawngreen,
  LemonChiffon,
  LightBlue,
  LightCoral,
  LightCyan,
  LightGoldenRodYellow,
  LightGray,
  LightGreen,
  LightPink,
  LightSalmon,
  LightSeaGreen,
  LightskyBlue,
  LightslateGray,
  LightSteelBlue,
  LightYellow,
  LimeGreen,
  Linen,
  Magenta,
  MediumAquaMarine,
  MediumBlue,
  MediumOrchid,
  MediumPurple,
  MediumSeaGreen,
  MediumslateBlue,
  MediumSpringGreen,
  MediumTurquoise,
  MediumVioletRed,
  MidnightBlue,
  Mintcream,
  Mistyrose,
  Moccasin,
  NavajoWhite,
  Oldlace,
  OliveDrab,
  OrangeRed,
  Orchid,
  PaleGoldenRod,
  PaleGreen,
  PaleTurquoise,
  PaleVioletRed,
  Papayawhip,
  Peachpuff,
  Peru,
  Pink,
  Plum,
  PowderBlue,
  Rosybrown,
  Royalblue,
  SaddleBrown,
  Salmon,
  SandyBrown,
  Seagreen,
  Seashell,
  Sienna,
  SkyBlue,
  SlateBlue,
  SlateGray,
  Snow,
  SpringGreen,
  SteelBlue,
  Tan,
  Thistle,
  Tomato,
  Turquoise,
  Violet,
  Wheat,
  WhiteSmoke,
  YellowGreen,
  MAX_VALUE
};

// RGB source
// http://pixeljoint.com/forum/forum_posts.asp?TID=12795
// http://www.color-blindness.com/color-name-hue/ gave names
enum class DawnbringerPalette
{
  Blackcurrant,
  Castro,
  ToreaBay,
  Liver,
  Korma,
  SanFelix,
  Valencia,
  Flint,
  HavelockBlue,
  Tango,
  BaliHai,
  Sushi,
  Cashmere,
  Seagull,
  Goldenrod,
  Tara,
  MAX_VALUE
};

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

class Rgba;

class Rgb
{
 public:
  Rgb(const float red, const float green, const float blue);
  explicit Rgb(const float gray);
  explicit Rgb(const int rgb);
  explicit Rgb(const Rgba& rgb);
  static const Rgb
  From(Color color);
  static const Rgb
  From(DawnbringerPalette color);

  const float
  GetRed() const;
  const float
  GetGreen() const;
  const float
  GetBlue() const;

  // const float* data() const;

  void
  SetRed(const float v);
  void
  SetGreen(const float v);
  void
  SetBlue(const float v);

  // float& operator[](int index);
  // const float& operator[](int index) const;

  void
  SetRgb(const float red, const float green, const float blue);
  void
  SetRgb(int rgb);

 private:
  float r;
  float g;
  float b;
};

struct RgbTransform
{
  static Rgb
  Transform(const Rgb& from, float v, const Rgb to);
};

class Rgba
{
 public:
  Rgba(const Rgb& rgb, const float alpha = 1.0f);
  Rgba(const float red, const float green, const float blue,
       const float alpha = 1.0f);
  explicit Rgba(const float gray, const float alpha = 1.0f);

  const float*
  GetData() const;

  const float
  GetRed() const;
  const float
  GetGreen() const;
  const float
  GetBlue() const;
  const float
  GetAlpha() const;

  void
  SetRed(const float v);
  void
  SetGreen(const float v);
  void
  SetBlue(const float v);
  void
  SetAlpha(const float v);

  void
  SetRgb(const float red, const float green, const float blue);

 private:
  float r;
  float g;
  float b;
  float a;
};

#endif  // CORE_RGB_H
