#ifndef CORE_NAMED_COLORS_H
#define CORE_NAMED_COLORS_H

#include "core/enumtostring.h"

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

BEGIN_ENUM_LIST(Color)
  ENUM_VALUE(Color, White)
  ENUM_VALUE(Color, Silver)
  ENUM_VALUE(Color, Gray)
  ENUM_VALUE(Color, Black)
  ENUM_VALUE(Color, Navy)
  ENUM_VALUE(Color, Blue)
  ENUM_VALUE(Color, Aqua)
  ENUM_VALUE(Color, Teal)
  ENUM_VALUE(Color, Green)
  ENUM_VALUE(Color, Olive)
  ENUM_VALUE(Color, Lime)
  ENUM_VALUE(Color, Maroon)
  ENUM_VALUE(Color, Red)
  ENUM_VALUE(Color, Orange)
  ENUM_VALUE(Color, Yellow)
  ENUM_VALUE(Color, Purple)
  ENUM_VALUE(Color, Fuchsia)
  ENUM_VALUE(Color, AliceBlue)
  ENUM_VALUE(Color, AntiqueWhite)
  ENUM_VALUE(Color, AquaMarine)
  ENUM_VALUE(Color, Azure)
  ENUM_VALUE(Color, Beige)
  ENUM_VALUE(Color, Bisque)
  ENUM_VALUE(Color, BlanchedAlmond)
  ENUM_VALUE(Color, BlueViolet)
  ENUM_VALUE(Color, Brown)
  ENUM_VALUE(Color, BurlyWood)
  ENUM_VALUE(Color, CadetBlue)
  ENUM_VALUE(Color, Chartreuse)
  ENUM_VALUE(Color, Chocolate)
  ENUM_VALUE(Color, Coral)
  ENUM_VALUE(Color, CornflowerBlue)
  ENUM_VALUE(Color, Cornsilk)
  ENUM_VALUE(Color, Crimson)
  ENUM_VALUE(Color, Cyan)
  ENUM_VALUE(Color, Darkblue)
  ENUM_VALUE(Color, Darkcyan)
  ENUM_VALUE(Color, DarkGoldenRod)
  ENUM_VALUE(Color, Darkgray)
  ENUM_VALUE(Color, Darkgreen)
  ENUM_VALUE(Color, DarKkhaki)
  ENUM_VALUE(Color, DarkMagenta)
  ENUM_VALUE(Color, DarkOliveGreen)
  ENUM_VALUE(Color, DarkOrange)
  ENUM_VALUE(Color, DarkOrchid)
  ENUM_VALUE(Color, DarkRed)
  ENUM_VALUE(Color, DarkSalmon)
  ENUM_VALUE(Color, DarkSeaGreen)
  ENUM_VALUE(Color, DarkslateBlue)
  ENUM_VALUE(Color, DarkslateGray)
  ENUM_VALUE(Color, DarkTurquoise)
  ENUM_VALUE(Color, DarkBiolet)
  ENUM_VALUE(Color, DeeppInk)
  ENUM_VALUE(Color, DeepskyBlue)
  ENUM_VALUE(Color, DimGray)
  ENUM_VALUE(Color, DodgerBlue)
  ENUM_VALUE(Color, FireBrick)
  ENUM_VALUE(Color, FloralWhite)
  ENUM_VALUE(Color, ForestGreen)
  ENUM_VALUE(Color, Gainsboro)
  ENUM_VALUE(Color, GhostWhite)
  ENUM_VALUE(Color, Gold)
  ENUM_VALUE(Color, GoldenRod)
  ENUM_VALUE(Color, GreenYellow)
  ENUM_VALUE(Color, Honeydew)
  ENUM_VALUE(Color, Hotpink)
  ENUM_VALUE(Color, IndianRed)
  ENUM_VALUE(Color, Indigo)
  ENUM_VALUE(Color, Ivory)
  ENUM_VALUE(Color, Khaki)
  ENUM_VALUE(Color, Lavender)
  ENUM_VALUE(Color, LavenderBlush)
  ENUM_VALUE(Color, Lawngreen)
  ENUM_VALUE(Color, LemonChiffon)
  ENUM_VALUE(Color, LightBlue)
  ENUM_VALUE(Color, LightCoral)
  ENUM_VALUE(Color, LightCyan)
  ENUM_VALUE(Color, LightGoldenRodYellow)
  ENUM_VALUE(Color, LightGray)
  ENUM_VALUE(Color, LightGreen)
  ENUM_VALUE(Color, LightPink)
  ENUM_VALUE(Color, LightSalmon)
  ENUM_VALUE(Color, LightSeaGreen)
  ENUM_VALUE(Color, LightskyBlue)
  ENUM_VALUE(Color, LightslateGray)
  ENUM_VALUE(Color, LightSteelBlue)
  ENUM_VALUE(Color, LightYellow)
  ENUM_VALUE(Color, LimeGreen)
  ENUM_VALUE(Color, Linen)
  ENUM_VALUE(Color, Magenta)
  ENUM_VALUE(Color, MediumAquaMarine)
  ENUM_VALUE(Color, MediumBlue)
  ENUM_VALUE(Color, MediumOrchid)
  ENUM_VALUE(Color, MediumPurple)
  ENUM_VALUE(Color, MediumSeaGreen)
  ENUM_VALUE(Color, MediumslateBlue)
  ENUM_VALUE(Color, MediumSpringGreen)
  ENUM_VALUE(Color, MediumTurquoise)
  ENUM_VALUE(Color, MediumVioletRed)
  ENUM_VALUE(Color, MidnightBlue)
  ENUM_VALUE(Color, Mintcream)
  ENUM_VALUE(Color, Mistyrose)
  ENUM_VALUE(Color, Moccasin)
  ENUM_VALUE(Color, NavajoWhite)
  ENUM_VALUE(Color, Oldlace)
  ENUM_VALUE(Color, OliveDrab)
  ENUM_VALUE(Color, OrangeRed)
  ENUM_VALUE(Color, Orchid)
  ENUM_VALUE(Color, PaleGoldenRod)
  ENUM_VALUE(Color, PaleGreen)
  ENUM_VALUE(Color, PaleTurquoise)
  ENUM_VALUE(Color, PaleVioletRed)
  ENUM_VALUE(Color, Papayawhip)
  ENUM_VALUE(Color, Peachpuff)
  ENUM_VALUE(Color, Peru)
  ENUM_VALUE(Color, Pink)
  ENUM_VALUE(Color, Plum)
  ENUM_VALUE(Color, PowderBlue)
  ENUM_VALUE(Color, Rosybrown)
  ENUM_VALUE(Color, Royalblue)
  ENUM_VALUE(Color, SaddleBrown)
  ENUM_VALUE(Color, Salmon)
  ENUM_VALUE(Color, SandyBrown)
  ENUM_VALUE(Color, Seagreen)
  ENUM_VALUE(Color, Seashell)
  ENUM_VALUE(Color, Sienna)
  ENUM_VALUE(Color, SkyBlue)
  ENUM_VALUE(Color, SlateBlue)
  ENUM_VALUE(Color, SlateGray)
  ENUM_VALUE(Color, Snow)
  ENUM_VALUE(Color, SpringGreen)
  ENUM_VALUE(Color, SteelBlue)
  ENUM_VALUE(Color, Tan)
  ENUM_VALUE(Color, Thistle)
  ENUM_VALUE(Color, Tomato)
  ENUM_VALUE(Color, Turquoise)
  ENUM_VALUE(Color, Violet)
  ENUM_VALUE(Color, Wheat)
  ENUM_VALUE(Color, WhiteSmoke)
  ENUM_VALUE(Color, YellowGreen)
END_ENUM_LIST()

unsigned int
ToColorHex(Color color);

#endif  // CORE_NAMED_COLORS_H
