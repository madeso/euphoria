#ifndef EUPHORIA_PALETTE_ALL_H
#define EUPHORIA_PALETTE_ALL_H

#include <string>

#include "core/enumtostring.h"

struct Palette;

namespace palette
{
  enum class PaletteName
  {
    // cubehelix
  Classic,
  Perceptual_rainbow,
  Purple,
  Jim_special,
  Red,
  Cubehelix1,
  Cubehelix2,
  Cubehelix3,

    // lospec
  OneBit, 
  TwoBitGrayScale, 
  ThreeBit, 
  ARQ4, 
  CGA, 
  EDG8, 
  EDG16, 
  EDG32, 
  EDG36, 
  EDG64, 
  EN4, 
  Ink, 
  Pico8, 
  Ammo8, 
  NYX8, 
  FifteenPDX, 
  TwentyPDX, 
  Arne16, 
  Night16, 
  AAP16, 
  AAP64, 
  Splendor128, 
  Famicube, 

  // matplot
  Magma,
  Inferno,
  Plasma,
  Viridis,

  // mycarta
  Cube1,
  CubeYF,
  LinearL,

  // tableu
  Tableau_10,
  TableauLight_10,
  TableauMedium_10,
  Tableau_20,
  Gray_5,
  ColorBlind_10,
  TrafficLight_9,
  PurpleGray_6,
  PurpleGray_12,
  BlueRed_6,
  BlueRed_12,
  GreenOrange_6,
  GreenOrange_12,


  // wes
   Chevalier,
   Moonrise1,
   Mendl,
   Margot1,
   Cavalcanti,
   Moonrise2,
   Margot2,
   Moonrise3,
   GrandBudapest1,
   Moonrise4,
   Zissou,
   Royal1,
   Darjeeling1,
   FantasticFox1,
   Margot3,
   GrandBudapest2,
   Aquatic1,
   Darjeeling2,
   FantasticFox2,
   GrandBudapest3,
   Royal2,
   Moonrise5,
   GrandBudapest4,
   Moonrise6,
   GrandBudapest5,
   Aquatic2,
   Royal3,
   Moonrise7,
   Aquatic3,
   Darjeeling3
  };

  const Palette& GetPalette(PaletteName name);
}
  BEGIN_ENUM_LIST(palette::PaletteName)
    // cubehelix
    ENUM_VALUE(palette::PaletteName, Classic)
    ENUM_VALUE(palette::PaletteName, Perceptual_rainbow)
    ENUM_VALUE(palette::PaletteName, Purple)
    ENUM_VALUE(palette::PaletteName, Jim_special)
    ENUM_VALUE(palette::PaletteName, Red)
    ENUM_VALUE(palette::PaletteName, Cubehelix1)
    ENUM_VALUE(palette::PaletteName, Cubehelix2)
    ENUM_VALUE(palette::PaletteName, Cubehelix3)

    // lospec
    ENUM_VALUE(palette::PaletteName, OneBit)
    ENUM_VALUE(palette::PaletteName, TwoBitGrayScale)
    ENUM_VALUE(palette::PaletteName, ThreeBit)
    ENUM_VALUE(palette::PaletteName, ARQ4)
    ENUM_VALUE(palette::PaletteName, CGA)
    ENUM_VALUE(palette::PaletteName, EDG8)
    ENUM_VALUE(palette::PaletteName, EDG16)
    ENUM_VALUE(palette::PaletteName, EDG32)
    ENUM_VALUE(palette::PaletteName, EDG36)
    ENUM_VALUE(palette::PaletteName, EDG64)
    ENUM_VALUE(palette::PaletteName, EN4)
    ENUM_VALUE(palette::PaletteName, Ink)
    ENUM_VALUE(palette::PaletteName, Pico8)
    ENUM_VALUE(palette::PaletteName, Ammo8)
    ENUM_VALUE(palette::PaletteName, NYX8)
    ENUM_VALUE(palette::PaletteName, FifteenPDX)
    ENUM_VALUE(palette::PaletteName, TwentyPDX)
    ENUM_VALUE(palette::PaletteName, Arne16)
    ENUM_VALUE(palette::PaletteName, Night16)
    ENUM_VALUE(palette::PaletteName, AAP16)
    ENUM_VALUE(palette::PaletteName, AAP64)
    ENUM_VALUE(palette::PaletteName, Splendor128)
    ENUM_VALUE(palette::PaletteName, Famicube)

    // matplot
    ENUM_VALUE(palette::PaletteName, Magma)
    ENUM_VALUE(palette::PaletteName, Inferno)
    ENUM_VALUE(palette::PaletteName, Plasma)
    ENUM_VALUE(palette::PaletteName, Viridis)

    // mycarta
    ENUM_VALUE(palette::PaletteName, Cube1)
    ENUM_VALUE(palette::PaletteName, CubeYF)
    ENUM_VALUE(palette::PaletteName, LinearL)

    // tableu
    ENUM_VALUE(palette::PaletteName, Tableau_10)
    ENUM_VALUE(palette::PaletteName, TableauLight_10)
    ENUM_VALUE(palette::PaletteName, TableauMedium_10)
    ENUM_VALUE(palette::PaletteName, Tableau_20)
    ENUM_VALUE(palette::PaletteName, Gray_5)
    ENUM_VALUE(palette::PaletteName, ColorBlind_10)
    ENUM_VALUE(palette::PaletteName, TrafficLight_9)
    ENUM_VALUE(palette::PaletteName, PurpleGray_6)
    ENUM_VALUE(palette::PaletteName, PurpleGray_12)
    ENUM_VALUE(palette::PaletteName, BlueRed_6)
    ENUM_VALUE(palette::PaletteName, BlueRed_12)
    ENUM_VALUE(palette::PaletteName, GreenOrange_6)
    ENUM_VALUE(palette::PaletteName, GreenOrange_12)

    // wes
    ENUM_VALUE(palette::PaletteName, Chevalier)
    ENUM_VALUE(palette::PaletteName, Moonrise1)
    ENUM_VALUE(palette::PaletteName, Mendl)
    ENUM_VALUE(palette::PaletteName, Margot1)
    ENUM_VALUE(palette::PaletteName, Cavalcanti)
    ENUM_VALUE(palette::PaletteName, Moonrise2)
    ENUM_VALUE(palette::PaletteName, Margot2)
    ENUM_VALUE(palette::PaletteName, Moonrise3)
    ENUM_VALUE(palette::PaletteName, GrandBudapest1)
    ENUM_VALUE(palette::PaletteName, Moonrise4)
    ENUM_VALUE(palette::PaletteName, Zissou)
    ENUM_VALUE(palette::PaletteName, Royal1)
    ENUM_VALUE(palette::PaletteName, Darjeeling1)
    ENUM_VALUE(palette::PaletteName, FantasticFox1)
    ENUM_VALUE(palette::PaletteName, Margot3)
    ENUM_VALUE(palette::PaletteName, GrandBudapest2)
    ENUM_VALUE(palette::PaletteName, Aquatic1)
    ENUM_VALUE(palette::PaletteName, Darjeeling2)
    ENUM_VALUE(palette::PaletteName, FantasticFox2)
    ENUM_VALUE(palette::PaletteName, GrandBudapest3)
    ENUM_VALUE(palette::PaletteName, Royal2)
    ENUM_VALUE(palette::PaletteName, Moonrise5)
    ENUM_VALUE(palette::PaletteName, GrandBudapest4)
    ENUM_VALUE(palette::PaletteName, Moonrise6)
    ENUM_VALUE(palette::PaletteName, GrandBudapest5)
    ENUM_VALUE(palette::PaletteName, Aquatic2)
    ENUM_VALUE(palette::PaletteName, Royal3)
    ENUM_VALUE(palette::PaletteName, Moonrise7)
    ENUM_VALUE(palette::PaletteName, Aquatic3)
    ENUM_VALUE(palette::PaletteName, Darjeeling3)
  END_ENUM_LIST()



#endif  // EUPHORIA_PALETTE_ALL_H

