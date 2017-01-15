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
  YellowGreen
};

class Rgb
{
 public:
  Rgb(const float r, const float g, const float b);
  explicit Rgb(const float gray);
  explicit Rgb(const int rgb);
  static const Rgb From(Color color);

  const float GetRed() const;
  const float GetGreen() const;
  const float GetBlue() const;

  //const float* data() const;

  void SetRed(const float v);
  void SetGreen(const float v);
  void SetBlue(const float v);

  //float& operator[](int index);
  //const float& operator[](int index) const;

  void SetRgb(const float r, const float g, const float b);
  void SetRgb(int rgb);
 private:
  float r;
  float g;
  float b;
};

struct RgbTransform {
  static Rgb Transform(const Rgb& from, float v, const Rgb to);
};

class Rgba
{
 public:
  Rgba(const Rgb& rgb, const float a=1.0f);
  Rgba(const float r, const float g, const float b, const float a=1.0f);
  explicit Rgba(const float gray, const float a=1.0f);

  const float* GetData() const;

  const float GetRed() const;
  const float GetGreen() const;
  const float GetBlue() const;
  const float GetAlpha() const;

  void SetRed(const float v);
  void SetGreen(const float v);
  void SetBlue(const float v);
  void SetAlpha(const float v);

  void SetRgb(const float r, const float g, const float b);
 private:
  float r;
  float g;
  float b;
  float a;
};

#endif  // CORE_RGB_H
