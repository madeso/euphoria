#include "core/palette.h"

#include "core/numeric.h"
#include "core/random.h"

namespace
{
  float
  Diff(const Rgb& lhs, const Rgb& rhs)
  {
    return Square(lhs.r - rhs.r) + Square(lhs.g - rhs.g) +
           Square(lhs.b - rhs.b);
  }
}

const Rgb&
Palette::GetRandomColor(Random* r) const
{
  return r->Next(colors);
}

const Rgb&
Palette::GetSafeIndex(unsigned int i) const
{
  return colors[i % colors.size()];
}

const Rgb&
Palette::GetClosestColor(const Rgb& c) const
{
  return colors[GetIndexClosest(c)];
}

unsigned int
Palette::GetIndexClosest(const Rgb& c) const
{
  ASSERT(!colors.empty());
  auto diff_best = Diff(c, colors[0]);

  unsigned int index_best = 0;
  const auto   size       = colors.size();
  for(unsigned int index = 1; index < size; index += 1)
  {
    const auto diff = Diff(c, colors[index]);
    if(diff < diff_best)
    {
      diff_best  = diff;
      index_best = index;
    }
  }

  return index_best;
}

////////////////////////////////////////////////////////////////////////////////

namespace
{
  Rgb
  C(unsigned int hex)
  {
    return Rgb::FromHex(hex);
  }
}

namespace palette
{
  const Palette&
  Dawnbringer()
  {
    static const auto p = Palette{
        C(0x140C1C),
        C(0x442434),
        C(0x30346D),
        C(0x4E4A4E),
        C(0x854C30),
        C(0x346524),
        C(0xD04648),
        C(0x757161),
        C(0x597DCE),
        C(0xD27D2C),
        C(0x8595A1),
        C(0x6DAA2C),
        C(0xD2AA99),
        C(0x6DC2CA),
        C(0xDAD45E),
        C(0xDEEED6),
    };
    return p;
  }

  const Palette&
  OneBit()
  {
    static const auto p = Palette{
        C(0x000000), C(0xFFFFFF),
    };
    return p;
  }

  const Palette&
  TwoBitGrayScale()
  {
    static const auto p = Palette{
        C(0x000000), C(0x676767), C(0xB6B6B6), C(0xFFFFFF),
    };
    return p;
  }

  const Palette&
  ThreeBit()
  {
    static const auto p = Palette{
        C(0x000000),
        C(0xFF0000),
        C(0x00FF00),
        C(0x0000FF),
        C(0x00FFFF),
        C(0xFF00FF),
        C(0xFFFF00),
        C(0xFFFFFF),
    };
    return p;
  }

  const Palette&
  ARQ4()
  {
    static const auto p = Palette{
        C(0xFFFFFF), C(0x6772A9), C(0x3A3277), C(0x000000),
    };
    return p;
  }

  const Palette&
  CGA()
  {
    static const auto p = Palette{
        C(0x000000),
        C(0x555555),
        C(0xAAAAAA),
        C(0xFFFFFF),
        C(0x0000AA),
        C(0x5555FF),
        C(0x00AA00),
        C(0x55FF55),
        C(0x00AAAA),
        C(0x55FFFF),
        C(0xAA0000),
        C(0xFF5555),
        C(0xAA00AA),
        C(0xFF55FF),
        C(0xAA5500),
        C(0xFFFF55),
    };
    return p;
  }

  const Palette&
  EDG8()
  {
    static const auto p = Palette{
        C(0xFDFDF8),
        C(0xD32734),
        C(0xDA7D22),
        C(0xE6DA29),
        C(0x28C641),
        C(0x2D93DD),
        C(0x7B53AD),
        C(0x1B1C33),
    };
    return p;
  }

  const Palette&
  EDG16()
  {
    static const auto p = Palette{
        C(0xE4A672),
        C(0xB86F50),
        C(0x743F39),
        C(0x3F2832),
        C(0x9E2835),
        C(0xE53B44),
        C(0xFB922B),
        C(0xFFE762),
        C(0x63C64D),
        C(0x327345),
        C(0x193D3F),
        C(0x4F6781),
        C(0xAFBFD2),
        C(0xFFFFFF),
        C(0x2CE8F4),
        C(0x0484D1),
    };
    return p;
  }

  const Palette&
  EDG32()
  {
    static const auto p = Palette{
        C(0xBE4A2F), C(0xD77643), C(0xEAD4AA), C(0xE4A672), C(0xB86F50),
        C(0x733E39), C(0x3E2731), C(0xA22633), C(0xE43B44), C(0xF77622),
        C(0xFEAE34), C(0xFEE761), C(0x63C74D), C(0x3E8948), C(0x265C42),
        C(0x193C3E), C(0x124E89), C(0x0099DB), C(0x2CE8F5), C(0xFFFFFF),
        C(0xC0CBDC), C(0x8B9BB4), C(0x5A6988), C(0x3A4466), C(0x262B44),
        C(0x181425), C(0xFF0044), C(0x68386C), C(0xB55088), C(0xF6757A),
        C(0xE8B796), C(0xC28569),
    };
    return p;
  }

  const Palette&
  EDG36()
  {
    static const auto p = Palette{
        C(0xDBE0E7), C(0xA3ACBE), C(0x67708B), C(0x4E5371), C(0x393A56),
        C(0x26243A), C(0x141020), C(0x7BCF5C), C(0x509B4B), C(0x2E6A42),
        C(0x1A453B), C(0x0F2738), C(0x0D2F6D), C(0x0F4DA3), C(0x0E82CE),
        C(0x13B2F2), C(0x41F3FC), C(0xF0D2AF), C(0xE5AE78), C(0xC58158),
        C(0x945542), C(0x623530), C(0x46211F), C(0x97432A), C(0xE57028),
        C(0xF7AC37), C(0xFBDF6B), C(0xFE979B), C(0xED5259), C(0xC42C36),
        C(0x781F2C), C(0x351428), C(0x4D2352), C(0x7F3B86), C(0xB45EB3),
        C(0xE38DD6),
    };
    return p;
  }

  const Palette&
  EDG64()
  {
    static const auto p = Palette{
        C(0xFF0040), C(0x131313), C(0x1B1B1B), C(0x272727), C(0x3D3D3D),
        C(0x5D5D5D), C(0x858585), C(0xB4B4B4), C(0xFFFFFF), C(0xC7CFDD),
        C(0x92A1B9), C(0x657392), C(0x424C6E), C(0x2A2F4E), C(0x1A1932),
        C(0x0E071B), C(0x1C121C), C(0x391F21), C(0x5D2C28), C(0x8A4836),
        C(0xBF6F4A), C(0xE69C69), C(0xF6CA9F), C(0xF9E6CF), C(0xEDAB50),
        C(0xE07438), C(0xC64524), C(0x8E251D), C(0xFF5000), C(0xED7614),
        C(0xFFA214), C(0xFFC825), C(0xFFEB57), C(0xD3FC7E), C(0x99E65F),
        C(0x5AC54F), C(0x33984B), C(0x1E6F50), C(0x134C4C), C(0x0C2E44),
        C(0x00396D), C(0x0069AA), C(0x0098DC), C(0x00CDF9), C(0x0CF1FF),
        C(0x94FDFF), C(0xFDD2ED), C(0xF389F5), C(0xDB3FFD), C(0x7A09FA),
        C(0x3003D9), C(0x0C0293), C(0x03193F), C(0x3B1443), C(0x622461),
        C(0x93388F), C(0xCA52C9), C(0xC85086), C(0xF68187), C(0xF5555D),
        C(0xEA323C), C(0xC42430), C(0x891E2B), C(0x571C27),
    };
    return p;
  }

  const Palette&
  EN4()
  {
    static const auto p = Palette{
        C(0xFBF7F3), C(0xE5B083), C(0x426E5D), C(0x20283D),
    };
    return p;
  }

  const Palette&
  Ink()
  {
    static const auto p = Palette{
        C(0x1F1F29), C(0x413A42), C(0x596070), C(0x96A2B3), C(0xEAF0D8),
    };
    return p;
  }

  const Palette&
  Pico8()
  {
    static const auto p = Palette{
        C(0x000000),
        C(0x5F574F),
        C(0xC2C3C7),
        C(0xFFF1E8),
        C(0xFFEC27),
        C(0xFFA300),
        C(0xFFCCAA),
        C(0xAB5236),
        C(0xFF77A8),
        C(0xFF004D),
        C(0x83769C),
        C(0x7E2553),
        C(0x29ADFF),
        C(0x1D2B53),
        C(0x008751),
        C(0x00E436),
    };
    return p;
  }

  const Palette&
  Ammo8()
  {
    static const auto p = Palette{
        C(0x040C06),
        C(0x112318),
        C(0x1E3A29),
        C(0x305D42),
        C(0x4D8061),
        C(0x89A257),
        C(0xBEDC7F),
        C(0xEEFFCC),
    };
    return p;
  }

  const Palette&
  NYX8()
  {
    static const auto p = Palette{
        C(0x08141E),
        C(0x0F2A3F),
        C(0x20394F),
        C(0xF6D6BD),
        C(0xC3A38A),
        C(0x997577),
        C(0x816271),
        C(0x4E495F),
    };
    return p;
  }

  const Palette&
  FifteenPDX()
  {
    static const auto p = Palette{
        C(0x6E3232),
        C(0xBB5735),
        C(0xDF9245),
        C(0xECD274),
        C(0x83A816),
        C(0x277224),
        C(0x173B47),
        C(0x046894),
        C(0x17A1A9),
        C(0x81DBCD),
        C(0xFDF9F1),
        C(0xC7B295),
        C(0x87715B),
        C(0x463F3C),
        C(0x201708),
    };
    return p;
  }

  const Palette&
  TwentyPDX()
  {
    static const auto p = Palette{
        C(0x170D20), C(0x474757), C(0x787876), C(0xB1B9A6), C(0xEBFFDA),
        C(0x68293E), C(0xA94400), C(0xD97E00), C(0xEBD000), C(0x523C14),
        C(0x816031), C(0xBC8B57), C(0xEBCD93), C(0x0E4C58), C(0x046E92),
        C(0x01A3C3), C(0x55DEB7), C(0x177947), C(0x5AB217), C(0xB1E329),
    };
    return p;
  }

  const Palette&
  Arne16()
  {
    static const auto p = Palette{
        C(0x000000),
        C(0x493C2B),
        C(0xBE2633),
        C(0xE06F8B),
        C(0x9D9D9D),
        C(0xA46422),
        C(0xEB8931),
        C(0xF7E26B),
        C(0xFFFFFF),
        C(0x1B2632),
        C(0x2F484E),
        C(0x44891A),
        C(0xA3CE27),
        C(0x005784),
        C(0x31A2F2),
        C(0xB2DCEF),
    };
    return p;
  }

  const Palette&
  Night16()
  {
    static const auto p = Palette{
        C(0x0F0F1E),
        C(0xFFF8BC),
        C(0x0C2133),
        C(0x48586D),
        C(0x79A0B0),
        C(0xB0CE9D),
        C(0x657F49),
        C(0x3F4536),
        C(0xB99D6A),
        C(0xFFDD91),
        C(0xDD945B),
        C(0x9A5142),
        C(0x644B48),
        C(0x333033),
        C(0x767088),
        C(0xC5A3B3),
    };
    return p;
  }

  const Palette&
  AAP16()
  {
    static const auto p = Palette{
        C(0x070708),
        C(0x332222),
        C(0x774433),
        C(0xCC8855),
        C(0x993311),
        C(0xDD7711),
        C(0xFFDD55),
        C(0xFFFF33),
        C(0x55AA44),
        C(0x115522),
        C(0x44EEBB),
        C(0x3388DD),
        C(0x5544AA),
        C(0x555577),
        C(0xAABBBB),
        C(0xFFFFFF),
    };
    return p;
  }

  const Palette&
  AAP64()
  {
    static const auto p = Palette{
        C(0x060608), C(0x141013), C(0x3B1725), C(0x73172D), C(0xB4202A),
        C(0xDF3E23), C(0xFA6A0A), C(0xF9A31B), C(0xFFD541), C(0xFFFC40),
        C(0xD6F264), C(0x9CDB43), C(0x59C135), C(0x14A02E), C(0x1A7A3E),
        C(0x24523B), C(0x122020), C(0x143464), C(0x285CC4), C(0x249FDE),
        C(0x20D6C7), C(0xA6FCDB), C(0xFFFFFF), C(0xFEF3C0), C(0xFAD6B8),
        C(0xF5A097), C(0xE86A73), C(0xBC4A9B), C(0x793A80), C(0x403353),
        C(0x242234), C(0x221C1A), C(0x322B28), C(0x71413B), C(0xBB7547),
        C(0xDBA463), C(0xF4D29C), C(0xDAE0EA), C(0xB3B9D1), C(0x8B93AF),
        C(0x6D758D), C(0x4A5462), C(0x333941), C(0x422433), C(0x5B3138),
        C(0x8E5252), C(0xBA756A), C(0xE9B5A3), C(0xE3E6FF), C(0xB9BFFB),
        C(0x849BE4), C(0x588DBE), C(0x477D85), C(0x23674E), C(0x328464),
        C(0x5DAF8D), C(0x92DCBA), C(0xCDF7E2), C(0xE4D2AA), C(0xC7B08B),
        C(0xA08662), C(0x796755), C(0x5A4E44), C(0x423934),
    };
    return p;
  }

  const Palette&
  Splendor128()
  {
    static const auto p = Palette{
        C(0x050403), C(0x0E0C0C), C(0x2D1B1E), C(0x612721), C(0xB9451D),
        C(0xF1641F), C(0xFCA570), C(0xFFE0B7), C(0xFFFFFF), C(0xFFF089),
        C(0xF8C53A), C(0xE88A36), C(0xB05B2C), C(0x673931), C(0x271F1B),
        C(0x4C3D2E), C(0x855F39), C(0xD39741), C(0xF8F644), C(0xD5DC1D),
        C(0xADB834), C(0x7F8E44), C(0x586335), C(0x333C24), C(0x181C19),
        C(0x293F21), C(0x477238), C(0x61A53F), C(0x8FD032), C(0xC4F129),
        C(0xD0FFEA), C(0x97EDCA), C(0x59CF93), C(0x42A459), C(0x3D6F43),
        C(0x27412D), C(0x14121D), C(0x1B2447), C(0x2B4E95), C(0x2789CD),
        C(0x42BFE8), C(0x73EFE8), C(0xF1F2FF), C(0xC9D4FD), C(0x8AA1F6),
        C(0x4572E3), C(0x494182), C(0x7864C6), C(0x9C8BDB), C(0xCEAAED),
        C(0xFAD6FF), C(0xEEB59C), C(0xD480BB), C(0x9052BC), C(0x171516),
        C(0x373334), C(0x695B59), C(0xB28B78), C(0xE2B27E), C(0xF6D896),
        C(0xFCF7BE), C(0xECEBE7), C(0xCBC6C1), C(0xA69E9A), C(0x807B7A),
        C(0x595757), C(0x323232), C(0x4F342F), C(0x8C5B3E), C(0xC68556),
        C(0xD6A851), C(0xB47538), C(0x724B2C), C(0x452A1B), C(0x61683A),
        C(0x939446), C(0xC6B858), C(0xEFDD91), C(0xB5E7CB), C(0x86C69A),
        C(0x5D9B79), C(0x486859), C(0x2C3B39), C(0x171819), C(0x2C3438),
        C(0x465456), C(0x64878C), C(0x8AC4C3), C(0xAFE9DF), C(0xDCEAEE),
        C(0xB8CCD8), C(0x88A3BC), C(0x5E718E), C(0x485262), C(0x282C3C),
        C(0x464762), C(0x696682), C(0x9A97B9), C(0xC5C7DD), C(0xE6E7F0),
        C(0xEEE6EA), C(0xE3CDDF), C(0xBFA5C9), C(0x87738F), C(0x564F5B),
        C(0x322F35), C(0x36282B), C(0x654956), C(0x966888), C(0xC090A9),
        C(0xD4B8B8), C(0xEAE0DD), C(0xF1EBDB), C(0xDDCEBF), C(0xBDA499),
        C(0x886E6A), C(0x594D4D), C(0x33272A), C(0xB29476), C(0xE1BF89),
        C(0xF8E398), C(0xFFE9E3), C(0xFDC9C9), C(0xF6A2A8), C(0xE27285),
        C(0xB25266), C(0x64364B), C(0x2A1E23),
    };
    return p;
  }

  const Palette&
  Famicube()
  {
    static const auto p = Palette{
        C(0x000000), C(0x00177D), C(0x024ACA), C(0x0084FF), C(0x5BA8FF),
        C(0x98DCFF), C(0x9BA0EF), C(0x6264DC), C(0x3D34A5), C(0x211640),
        C(0x5A1991), C(0x6A31CA), C(0xA675FE), C(0xE2C9FF), C(0xFEC9ED),
        C(0xD59CFC), C(0xCC69E4), C(0xA328B3), C(0x871646), C(0xCF3C71),
        C(0xFF82CE), C(0xFFE9C5), C(0xF5B784), C(0xE18289), C(0xDA655E),
        C(0x823C3D), C(0x4F1507), C(0xE03C28), C(0xE2D7B5), C(0xC59782),
        C(0xAE6C37), C(0x5C3C0D), C(0x231712), C(0xAD4E1A), C(0xF68F37),
        C(0xFFE737), C(0xFFBB31), C(0xCC8F15), C(0x939717), C(0xB6C121),
        C(0xEEFFA9), C(0xBEEB71), C(0x8CD612), C(0x6AB417), C(0x376D03),
        C(0x172808), C(0x004E00), C(0x139D08), C(0x58D332), C(0x20B562),
        C(0x00604B), C(0x005280), C(0x0A98AC), C(0x25E2CD), C(0xBDFFCA),
        C(0x71A6A1), C(0x415D66), C(0x0D2030), C(0x151515), C(0x343434),
        C(0x7B7B7B), C(0xA8A8A8), C(0xD7D7D7), C(0xFFFFFF),
    };
    return p;
  }
}
