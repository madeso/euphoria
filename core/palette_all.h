#ifndef EUPHORIA_PALETTE_ALL_H
#define EUPHORIA_PALETTE_ALL_H

#include <string>

#include "core/enumtostring.h"

namespace euphoria::core
{

struct Palette;

namespace palette
{
  enum class PaletteName
  {
    // standard
    Dawnbringer,
    NamedColors,

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

DECLARE_ENUM_LIST(palette::PaletteName)

}

#endif  // EUPHORIA_PALETTE_ALL_H

