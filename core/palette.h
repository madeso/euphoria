#ifndef EUPHORIA_PALETTE_H
#define EUPHORIA_PALETTE_H

#include "core/assert.h"

#include "core/rgb.h"

class Random;

struct Palette
{
  std::vector<Rgbi> colors;

  template <typename... R>
  explicit Palette(R... c)
      : colors{c...}
  {
  }

  const Rgbi&
  GetRandomColor(Random* r) const;

  const Rgbi&
  GetSafeIndex(unsigned int i) const;

  const Rgbi&
  GetClosestColor(const Rgbi& c) const;

  unsigned int
  GetIndexClosest(const Rgbi& c) const;

  // rainbow functions based on the r documentation https://rdrr.io/r/grDevices/palettes.html

  static Palette
  Rainbow(int count, float saturation = 1, float lightness = 1);
  
  static Palette
  Rainbow(int count, const Angle& from, const Angle& to, float saturation, float lightness);
};

namespace palette
{
  // most of theese palettes was collected by
  // https://github.com/peterhellberg/gfx/blob/master/palettes.go

  // http://pixeljoint.com/forum/forum_posts.asp?TID=12795
  const Palette&
  Dawnbringer();

  // A basic 1-bit (black and white) palette.
  const Palette&
  OneBit();

  // A grayscale palette calculated using 2-bits per color.
  // It was used by the original gameboy and a few other computer systems.
  // https://lospec.com/palette-list/2-bit-grayscale
  const Palette&
  TwoBitGrayScale();

  // A calculated palette using 1 bit for each RGB value.
  // It was used by a number of early computers.
  const Palette&
  ThreeBit();

  // Created by Endesga. #ARQ4
  // https://lospec.com/palette-list/arq4
  const Palette&
  ARQ4();

  // Color Graphics Adapter Palette.
  // CGA was a graphics card released in 1981 for the IBM PC.
  // The standard mode uses one of two 4-color palettes
  // (each with a low-intensity and high-intensity mode),
  // but a hack allows use of all 16. #cga
  // https://lospec.com/palette-list/color-graphics-adapter
  const Palette&
  CGA();

  // Endesga 8 Palette.
  // Created by Endesga. #EDG8
  // https://lospec.com/palette-list/endesga-8
  const Palette&
  EDG8();

  // Endesga 16 Palette.
  // Created by Endesga. #EDG16
  // https://lospec.com/palette-list/endesga-16
  const Palette&
  EDG16();

  // Endesga 32 Palette.
  // Created by Endesga for his game NYKRA. #EDG32
  // https://lospec.com/palette-list/endesga-32
  const Palette&
  EDG32();

  // Endesga 36 Palette.
  // Created by Endesga. #EDG36
  // https://lospec.com/palette-list/endesga-36
  const Palette&
  EDG36();

  // Endesga 64 Palette.
  // "Honed over years of palette creation, refined for materialistic pixelart
  // and design. High contrast, high saturation, shaped around painting the
  // organic and structured life of the heptaverse." Created by Endesga.
  // #EDG64
  // https://lospec.com/palette-list/endesga-64
  const Palette&
  EDG64();

  // Created by Endesga. #EN4
  // https://lospec.com/palette-list/en4
  const Palette&
  EN4();

  // Created by AprilSundae.
  // https://lospec.com/palette-list/ink
  const Palette&
  Ink();

  // The palette used by PICO-8.
  // The PICO-8 is a virtual video game console created by Lexaloffle Games.
  // https://lospec.com/palette-list/pico-8
  const Palette&
  Pico8();

  // Ammo-8 created by rsvp asap.
  // https://lospec.com/palette-list/ammo-8
  const Palette&
  Ammo8();

  // Created by Javier Guerrero.
  // https://lospec.com/palette-list/nyx8
  const Palette&
  NYX8();

  // 15P DX palette by GrafxKid.
  // https://lospec.com/palette-list/15p-dx
  const Palette&
  FifteenPDX();

  // 20P DX palette by GrafxKid.
  // https://lospec.com/palette-list/20p-dx
  const Palette&
  TwentyPDX();

  // Created by Arne.
  // https://lospec.com/palette-list/arne-16
  const Palette&
  Arne16();

  // 3rd place winner of the PixelJoint 16 color palette competition (2015).
  // Created by Night.
  // https://lospec.com/palette-list/night-16
  const Palette&
  Night16();

  // AAP-16 created by Adigun Polack, meant for beginners.
  // https://lospec.com/palette-list/aap-16
  const Palette&
  AAP16();

  // AAP-64 palette created by Adigun Polack.
  // https://lospec.com/palette-list/aap-16
  const Palette&
  AAP64();

  // Splendor 128 palette created by Adigun Polack as a successor to
  // his AAP-64 palette. #Splendor128
  // https://lospec.com/palette-list/aap-splendor128
  const Palette&
  Splendor128();

  // Famicube palette created by Arne as part of his Famicube Project.
  // https://lospec.com/palette-list/famicube
  const Palette&
  Famicube();
}


#endif  // EUPHORIA_PALETTE_H
