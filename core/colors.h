#ifndef CORE_NAMED_COLORS_H
#define CORE_NAMED_COLORS_H

#include <string>
#include <vector>

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

// RGB source: http://pixeljoint.com/forum/forum_posts.asp?TID=12795
// gave names: http://www.color-blindness.com/color-name-hue/
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

namespace color
{
  bool
  IsValidLowerCase(const std::string& name);

  Color
  GetColorFromLowerCaseString(const std::string& name);

  std::vector<std::string>
  ListAllColorNames();
}

unsigned int
ToColorHex(Color color);

unsigned int
ToColorHex(DawnbringerPalette color);

#endif  // CORE_NAMED_COLORS_H
