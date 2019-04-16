#include "core/colorbrewer.h"

#include "core/palette.h"

#include <map>

/*
 * This product includes color specifications and designs developed by Cynthia
 * Brewer (http://colorbrewer.org/).
 
 https://groups.google.com/forum/?fromgroups=#!topic/d3-js/iyXFgJR1JY0
 */

namespace
{
  Rgbi
  rgb(unsigned char r, unsigned char g, unsigned char b)
  {
    return Rgbi{r, g, b};
  }
}  // namespace

namespace colorbrewer
{

  const std::vector<ColorBrewer>& All()
  {
    static const std::vector<ColorBrewer> all = {
      /*** Diverging ***/
      Spectral(),
      RdYlGn(),
      RdBu(),
      PiYG(),
      PRGn(),
      RdYlBu(),
      BrBG(),
      RdGy(),
      PuOr(),

      /*** Qualitative ***/
      Set2(),
      Accent(),
      Set1(),
      Set3(),
      Dark2(),
      Paired(),
      Pastel2(),
      Pastel1(),

      /*** Sequential ***/
      OrRd(),
      PuBu(),
      BuPu(),
      Oranges(),
      BuGn(),
      YlOrBr(),
      YlGn(),
      Reds(),
      RdPu(),
      Greens(),
      YlGnBu(),
      Purples(),
      GnBu(),
      Greys(),
      YlOrRd(),
      PuRd(),
      Blues(),
      PuBuGn()
    };
    return all;
  }

  const std::vector<SinglePalette> Find(Type* type, int* size)
  {
    // todo: implement better search
    std::vector<SinglePalette> r;
    for(const auto& br: All())
    {
      if(type)
      {
        if( br.type != *type ) continue;
      }
      if(size)
      {
        auto found = br.palettes.find(*size);
        if(found != br.palettes.end())
        {
          r.push_back(SinglePalette{ br.name, found->second});
        }
      }
      else
      {
        for(const auto& p: br.palettes)
        {
          r.push_back(SinglePalette{ br.name, p.second});
        }
      }
    }
    return r;
  }

/*** Diverging ***/
const ColorBrewer& Spectral() {
  static const ColorBrewer cb = ColorBrewer { "Spectral",
    Map {
        {3,  BrewerPalette{Palette{rgb(252,141,89), rgb(255,255,191), rgb(153,213,148)}}},
        {4,  BrewerPalette{Palette{rgb(215,25,28), rgb(253,174,97), rgb(171,221,164), rgb(43,131,186)}}},
        {5,  BrewerPalette{Palette{rgb(215,25,28), rgb(253,174,97), rgb(255,255,191), rgb(171,221,164), rgb(43,131,186)}}},
        {6,  BrewerPalette{Palette{rgb(213,62,79), rgb(252,141,89), rgb(254,224,139), rgb(230,245,152), rgb(153,213,148), rgb(50,136,189)}}},
        {7,  BrewerPalette{Palette{rgb(213,62,79), rgb(252,141,89), rgb(254,224,139), rgb(255,255,191), rgb(230,245,152), rgb(153,213,148), rgb(50,136,189)}}},
        {8,  BrewerPalette{Palette{rgb(213,62,79), rgb(244,109,67), rgb(253,174,97), rgb(254,224,139), rgb(230,245,152), rgb(171,221,164), rgb(102,194,165), rgb(50,136,189)}}},
        {9,  BrewerPalette{Palette{rgb(213,62,79), rgb(244,109,67), rgb(253,174,97), rgb(254,224,139), rgb(255,255,191), rgb(230,245,152), rgb(171,221,164), rgb(102,194,165), rgb(50,136,189)}}},
        {10, BrewerPalette{Palette{rgb(158,1,66), rgb(213,62,79), rgb(244,109,67), rgb(253,174,97), rgb(254,224,139), rgb(230,245,152), rgb(171,221,164), rgb(102,194,165), rgb(50,136,189), rgb(94,79,162)}}},
        {11, BrewerPalette{Palette{rgb(158,1,66), rgb(213,62,79), rgb(244,109,67), rgb(253,174,97), rgb(254,224,139), rgb(255,255,191), rgb(230,245,152), rgb(171,221,164), rgb(102,194,165), rgb(50,136,189), rgb(94,79,162)}}}
    }, Type::div, Properties{ Blind{2,2,2,0,0,0,0,0,0},Print{1,1,1,0,0,0,0,0,0},Copy{1,1,1,0,0,0,0,0,0},Screen{1,1,2,0,0,0,0,0,0} } };
  return cb;
}

const ColorBrewer& RdYlGn()   {
  static const ColorBrewer cb = ColorBrewer { "RdYlGn",
    Map {
      {3,  BrewerPalette{Palette{rgb(252,141,89), rgb(255,255,191), rgb(145,207,96)}}},
      {4,  BrewerPalette{Palette{rgb(215,25,28), rgb(253,174,97), rgb(166,217,106), rgb(26,150,65)}}},
      {5,  BrewerPalette{Palette{rgb(215,25,28), rgb(253,174,97), rgb(255,255,191), rgb(166,217,106), rgb(26,150,65)}}},
      {6,  BrewerPalette{Palette{rgb(215,48,39), rgb(252,141,89), rgb(254,224,139), rgb(217,239,139), rgb(145,207,96), rgb(26,152,80)}}},
      {7,  BrewerPalette{Palette{rgb(215,48,39), rgb(252,141,89), rgb(254,224,139), rgb(255,255,191), rgb(217,239,139), rgb(145,207,96), rgb(26,152,80)}}},
      {8,  BrewerPalette{Palette{rgb(215,48,39), rgb(244,109,67), rgb(253,174,97), rgb(254,224,139), rgb(217,239,139), rgb(166,217,106), rgb(102,189,99), rgb(26,152,80)}}},
      {9,  BrewerPalette{Palette{rgb(215,48,39), rgb(244,109,67), rgb(253,174,97), rgb(254,224,139), rgb(255,255,191), rgb(217,239,139), rgb(166,217,106), rgb(102,189,99), rgb(26,152,80)}}},
      {10, BrewerPalette{Palette{rgb(165,0,38), rgb(215,48,39), rgb(244,109,67), rgb(253,174,97), rgb(254,224,139), rgb(217,239,139), rgb(166,217,106), rgb(102,189,99), rgb(26,152,80), rgb(0,104,55)}}},
      {11, BrewerPalette{Palette{rgb(165,0,38), rgb(215,48,39), rgb(244,109,67), rgb(253,174,97), rgb(254,224,139), rgb(255,255,191), rgb(217,239,139), rgb(166,217,106), rgb(102,189,99), rgb(26,152,80), rgb(0,104,55)}}}
    }, Type::div, Properties{ Blind{2,2,2,0,0,0,0,0,0},Print{1,1,1,2,0,0,0,0,0},Copy{0},Screen{1,1,1,0,0,0,0,0,0} } };
  return cb;
}

const ColorBrewer& RdBu()     {
  static const ColorBrewer cb = ColorBrewer { "RdBu",
    Map {
      {3,  BrewerPalette{Palette{rgb(239,138,98), rgb(247,247,247), rgb(103,169,207)}}},
      {4,  BrewerPalette{Palette{rgb(202,0,32), rgb(244,165,130), rgb(146,197,222), rgb(5,113,176)}}},
      {5,  BrewerPalette{Palette{rgb(202,0,32), rgb(244,165,130), rgb(247,247,247), rgb(146,197,222), rgb(5,113,176)}}},
      {6,  BrewerPalette{Palette{rgb(178,24,43), rgb(239,138,98), rgb(253,219,199), rgb(209,229,240), rgb(103,169,207), rgb(33,102,172)}}},
      {7,  BrewerPalette{Palette{rgb(178,24,43), rgb(239,138,98), rgb(253,219,199), rgb(247,247,247), rgb(209,229,240), rgb(103,169,207), rgb(33,102,172)}}},
      {8,  BrewerPalette{Palette{rgb(178,24,43), rgb(214,96,77), rgb(244,165,130), rgb(253,219,199), rgb(209,229,240), rgb(146,197,222), rgb(67,147,195), rgb(33,102,172)}}},
      {9,  BrewerPalette{Palette{rgb(178,24,43), rgb(214,96,77), rgb(244,165,130), rgb(253,219,199), rgb(247,247,247), rgb(209,229,240), rgb(146,197,222), rgb(67,147,195), rgb(33,102,172)}}},
      {10, BrewerPalette{Palette{rgb(103,0,31), rgb(178,24,43), rgb(214,96,77), rgb(244,165,130), rgb(253,219,199), rgb(209,229,240), rgb(146,197,222), rgb(67,147,195), rgb(33,102,172), rgb(5,48,97)}}},
      {11, BrewerPalette{Palette{rgb(103,0,31), rgb(178,24,43), rgb(214,96,77), rgb(244,165,130), rgb(253,219,199), rgb(247,247,247), rgb(209,229,240), rgb(146,197,222), rgb(67,147,195), rgb(33,102,172), rgb(5,48,97)}}}
    }, Type::div, Properties{Blind{1},Print{1,1,1,1,0,0,0,0,0},Copy{0},Screen{1,1,1,0,0,0,0,0,0} } };
  return cb;
}

const ColorBrewer& PiYG()     {
  static const ColorBrewer cb = ColorBrewer { "PiYG",
    Map {
      {3,  BrewerPalette{Palette{rgb(233,163,201), rgb(247,247,247), rgb(161,215,106)}}},
      {4,  BrewerPalette{Palette{rgb(208,28,139), rgb(241,182,218), rgb(184,225,134), rgb(77,172,38)}}},
      {5,  BrewerPalette{Palette{rgb(208,28,139), rgb(241,182,218), rgb(247,247,247), rgb(184,225,134), rgb(77,172,38)}}},
      {6,  BrewerPalette{Palette{rgb(197,27,125), rgb(233,163,201), rgb(253,224,239), rgb(230,245,208), rgb(161,215,106), rgb(77,146,33)}}},
      {7,  BrewerPalette{Palette{rgb(197,27,125), rgb(233,163,201), rgb(253,224,239), rgb(247,247,247), rgb(230,245,208), rgb(161,215,106), rgb(77,146,33)}}},
      {8,  BrewerPalette{Palette{rgb(197,27,125), rgb(222,119,174), rgb(241,182,218), rgb(253,224,239), rgb(230,245,208), rgb(184,225,134), rgb(127,188,65), rgb(77,146,33)}}},
      {9,  BrewerPalette{Palette{rgb(197,27,125), rgb(222,119,174), rgb(241,182,218), rgb(253,224,239), rgb(247,247,247), rgb(230,245,208), rgb(184,225,134), rgb(127,188,65), rgb(77,146,33)}}},
      {10, BrewerPalette{Palette{rgb(142,1,82), rgb(197,27,125), rgb(222,119,174), rgb(241,182,218), rgb(253,224,239), rgb(230,245,208), rgb(184,225,134), rgb(127,188,65), rgb(77,146,33), rgb(39,100,25)}}},
      {11, BrewerPalette{Palette{rgb(142,1,82), rgb(197,27,125), rgb(222,119,174), rgb(241,182,218), rgb(253,224,239), rgb(247,247,247), rgb(230,245,208), rgb(184,225,134), rgb(127,188,65), rgb(77,146,33), rgb(39,100,25)}}}
    }, Type::div, Properties{Blind{1},Print{1,1,2,0,0,0,0,0,0},Copy{0},Screen{1,1,2,0,0,0,0,0,0} } };
  return cb;
}

const ColorBrewer& PRGn()     {
  static const ColorBrewer cb = ColorBrewer { "PRGn",
    Map {
      {3,  BrewerPalette{Palette{rgb(175,141,195), rgb(247,247,247), rgb(127,191,123)}}},
      {4,  BrewerPalette{Palette{rgb(123,50,148), rgb(194,165,207), rgb(166,219,160), rgb(0,136,55)}}},
      {5,  BrewerPalette{Palette{rgb(123,50,148), rgb(194,165,207), rgb(247,247,247), rgb(166,219,160), rgb(0,136,55)}}},
      {6,  BrewerPalette{Palette{rgb(118,42,131), rgb(175,141,195), rgb(231,212,232), rgb(217,240,211), rgb(127,191,123), rgb(27,120,55)}}},
      {7,  BrewerPalette{Palette{rgb(118,42,131), rgb(175,141,195), rgb(231,212,232), rgb(247,247,247), rgb(217,240,211), rgb(127,191,123), rgb(27,120,55)}}},
      {8,  BrewerPalette{Palette{rgb(118,42,131), rgb(153,112,171), rgb(194,165,207), rgb(231,212,232), rgb(217,240,211), rgb(166,219,160), rgb(90,174,97), rgb(27,120,55)}}},
      {9,  BrewerPalette{Palette{rgb(118,42,131), rgb(153,112,171), rgb(194,165,207), rgb(231,212,232), rgb(247,247,247), rgb(217,240,211), rgb(166,219,160), rgb(90,174,97), rgb(27,120,55)}}},
      {10, BrewerPalette{Palette{rgb(64,0,75), rgb(118,42,131), rgb(153,112,171), rgb(194,165,207), rgb(231,212,232), rgb(217,240,211), rgb(166,219,160), rgb(90,174,97), rgb(27,120,55), rgb(0,68,27)}}},
      {11, BrewerPalette{Palette{rgb(64,0,75), rgb(118,42,131), rgb(153,112,171), rgb(194,165,207), rgb(231,212,232), rgb(247,247,247), rgb(217,240,211), rgb(166,219,160), rgb(90,174,97), rgb(27,120,55), rgb(0,68,27)}}}
    }, Type::div, Properties{Blind{1},Print{1,1,1,1,0,0,0,0,0},Copy{0},Screen{1,1,2,2,0,0,0,0,0} } };
  return cb;
}

const ColorBrewer& RdYlBu()   {
  static const ColorBrewer cb = ColorBrewer { "RdYlBu",
    Map {
      {3,  BrewerPalette{Palette{rgb(252,141,89), rgb(255,255,191), rgb(145,191,219)}}},
      {4,  BrewerPalette{Palette{rgb(215,25,28), rgb(253,174,97), rgb(171,217,233), rgb(44,123,182)}}},
      {5,  BrewerPalette{Palette{rgb(215,25,28), rgb(253,174,97), rgb(255,255,191), rgb(171,217,233), rgb(44,123,182)}}},
      {6,  BrewerPalette{Palette{rgb(215,48,39), rgb(252,141,89), rgb(254,224,144), rgb(224,243,248), rgb(145,191,219), rgb(69,117,180)}}},
      {7,  BrewerPalette{Palette{rgb(215,48,39), rgb(252,141,89), rgb(254,224,144), rgb(255,255,191), rgb(224,243,248), rgb(145,191,219), rgb(69,117,180)}}},
      {8,  BrewerPalette{Palette{rgb(215,48,39), rgb(244,109,67), rgb(253,174,97), rgb(254,224,144), rgb(224,243,248), rgb(171,217,233), rgb(116,173,209), rgb(69,117,180)}}},
      {9,  BrewerPalette{Palette{rgb(215,48,39), rgb(244,109,67), rgb(253,174,97), rgb(254,224,144), rgb(255,255,191), rgb(224,243,248), rgb(171,217,233), rgb(116,173,209), rgb(69,117,180)}}},
      {10, BrewerPalette{Palette{rgb(165,0,38), rgb(215,48,39), rgb(244,109,67), rgb(253,174,97), rgb(254,224,144), rgb(224,243,248), rgb(171,217,233), rgb(116,173,209), rgb(69,117,180), rgb(49,54,149)}}},
      {11, BrewerPalette{Palette{rgb(165,0,38), rgb(215,48,39), rgb(244,109,67), rgb(253,174,97), rgb(254,224,144), rgb(255,255,191), rgb(224,243,248), rgb(171,217,233), rgb(116,173,209), rgb(69,117,180), rgb(49,54,149)}}}
    }, Type::div, Properties{Blind{1},Print{1,1,1,1,2,0,0,0,0},Copy{0},Screen{1,1,1,2,0,0,0,0,0} } };
  return cb;
}

const ColorBrewer& BrBG()     {
  static const ColorBrewer cb = ColorBrewer { "BrBG",
    Map {
      {3,  BrewerPalette{Palette{rgb(216,179,101), rgb(245,245,245), rgb(90,180,172)}}},
      {4,  BrewerPalette{Palette{rgb(166,97,26), rgb(223,194,125), rgb(128,205,193), rgb(1,133,113)}}},
      {5,  BrewerPalette{Palette{rgb(166,97,26), rgb(223,194,125), rgb(245,245,245), rgb(128,205,193), rgb(1,133,113)}}},
      {6,  BrewerPalette{Palette{rgb(140,81,10), rgb(216,179,101), rgb(246,232,195), rgb(199,234,229), rgb(90,180,172), rgb(1,102,94)}}},
      {7,  BrewerPalette{Palette{rgb(140,81,10), rgb(216,179,101), rgb(246,232,195), rgb(245,245,245), rgb(199,234,229), rgb(90,180,172), rgb(1,102,94)}}},
      {8,  BrewerPalette{Palette{rgb(140,81,10), rgb(191,129,45), rgb(223,194,125), rgb(246,232,195), rgb(199,234,229), rgb(128,205,193), rgb(53,151,143), rgb(1,102,94)}}},
      {9,  BrewerPalette{Palette{rgb(140,81,10), rgb(191,129,45), rgb(223,194,125), rgb(246,232,195), rgb(245,245,245), rgb(199,234,229), rgb(128,205,193), rgb(53,151,143), rgb(1,102,94)}}},
      {10, BrewerPalette{Palette{rgb(84,48,5), rgb(140,81,10), rgb(191,129,45), rgb(223,194,125), rgb(246,232,195), rgb(199,234,229), rgb(128,205,193), rgb(53,151,143), rgb(1,102,94), rgb(0,60,48)}}},
      {11, BrewerPalette{Palette{rgb(84,48,5), rgb(140,81,10), rgb(191,129,45), rgb(223,194,125), rgb(246,232,195), rgb(245,245,245), rgb(199,234,229), rgb(128,205,193), rgb(53,151,143), rgb(1,102,94), rgb(0,60,48)}}}
    }, Type::div, Properties{Blind{1},Print{1,1,1,1,0,0,0,0,0},Copy{0},Screen{1,1,1,1,0,0,0,0,0} } };
  return cb;
}

const ColorBrewer& RdGy()     {
  static const ColorBrewer cb = ColorBrewer { "RdGy",
    Map {
      {3,  BrewerPalette{Palette{rgb(239,138,98), rgb(255,255,255), rgb(153,153,153)}}},
      {4,  BrewerPalette{Palette{rgb(202,0,32), rgb(244,165,130), rgb(186,186,186), rgb(64,64,64)}}},
      {5,  BrewerPalette{Palette{rgb(202,0,32), rgb(244,165,130), rgb(255,255,255), rgb(186,186,186), rgb(64,64,64)}}},
      {6,  BrewerPalette{Palette{rgb(178,24,43), rgb(239,138,98), rgb(253,219,199), rgb(224,224,224), rgb(153,153,153), rgb(77,77,77)}}},
      {7,  BrewerPalette{Palette{rgb(178,24,43), rgb(239,138,98), rgb(253,219,199), rgb(255,255,255), rgb(224,224,224), rgb(153,153,153), rgb(77,77,77)}}},
      {8,  BrewerPalette{Palette{rgb(178,24,43), rgb(214,96,77), rgb(244,165,130), rgb(253,219,199), rgb(224,224,224), rgb(186,186,186), rgb(135,135,135), rgb(77,77,77)}}},
      {9,  BrewerPalette{Palette{rgb(178,24,43), rgb(214,96,77), rgb(244,165,130), rgb(253,219,199), rgb(255,255,255), rgb(224,224,224), rgb(186,186,186), rgb(135,135,135), rgb(77,77,77)}}},
      {10, BrewerPalette{Palette{rgb(103,0,31), rgb(178,24,43), rgb(214,96,77), rgb(244,165,130), rgb(253,219,199), rgb(224,224,224), rgb(186,186,186), rgb(135,135,135), rgb(77,77,77), rgb(26,26,26)}}},
      {11, BrewerPalette{Palette{rgb(103,0,31), rgb(178,24,43), rgb(214,96,77), rgb(244,165,130), rgb(253,219,199), rgb(255,255,255), rgb(224,224,224), rgb(186,186,186), rgb(135,135,135), rgb(77,77,77), rgb(26,26,26)}}}
    }, Type::div, Properties{Blind{2},Print{1,1,1,2,0,0,0,0,0},Copy{0},Screen{1,1,2,0,0,0,0,0,0} } };
  return cb;
}

const ColorBrewer& PuOr()     {
  static const ColorBrewer cb = ColorBrewer { "PuOr",
    Map {
      {3,  BrewerPalette{Palette{rgb(241,163,64), rgb(247,247,247), rgb(153,142,195)}}},
      {4,  BrewerPalette{Palette{rgb(230,97,1), rgb(253,184,99), rgb(178,171,210), rgb(94,60,153)}}},
      {5,  BrewerPalette{Palette{rgb(230,97,1), rgb(253,184,99), rgb(247,247,247), rgb(178,171,210), rgb(94,60,153)}}},
      {6,  BrewerPalette{Palette{rgb(179,88,6), rgb(241,163,64), rgb(254,224,182), rgb(216,218,235), rgb(153,142,195), rgb(84,39,136)}}},
      {7,  BrewerPalette{Palette{rgb(179,88,6), rgb(241,163,64), rgb(254,224,182), rgb(247,247,247), rgb(216,218,235), rgb(153,142,195), rgb(84,39,136)}}},
      {8,  BrewerPalette{Palette{rgb(179,88,6), rgb(224,130,20), rgb(253,184,99), rgb(254,224,182), rgb(216,218,235), rgb(178,171,210), rgb(128,115,172), rgb(84,39,136)}}},
      {9,  BrewerPalette{Palette{rgb(179,88,6), rgb(224,130,20), rgb(253,184,99), rgb(254,224,182), rgb(247,247,247), rgb(216,218,235), rgb(178,171,210), rgb(128,115,172), rgb(84,39,136)}}},
      {10, BrewerPalette{Palette{rgb(127,59,8), rgb(179,88,6), rgb(224,130,20), rgb(253,184,99), rgb(254,224,182), rgb(216,218,235), rgb(178,171,210), rgb(128,115,172), rgb(84,39,136), rgb(45,0,75)}}},
      {11, BrewerPalette{Palette{rgb(127,59,8), rgb(179,88,6), rgb(224,130,20), rgb(253,184,99), rgb(254,224,182), rgb(247,247,247), rgb(216,218,235), rgb(178,171,210), rgb(128,115,172), rgb(84,39,136), rgb(45,0,75)}}}
    }, Type::div, Properties{Blind{1},Print{1,1,2,2,0,0,0,0,0},Copy{1,1,0,0,0,0,0,0,0},Screen{1,1,1,1,0,0,0,0,0} } };
  return cb;
}


/*** Qualitative ***/
const ColorBrewer& Set2()    {
  static const ColorBrewer cb = ColorBrewer { "Set2",
    Map {
      {3, BrewerPalette{Palette{rgb(102,194,165), rgb(252,141,98), rgb(141,160,203)}}},
      {4, BrewerPalette{Palette{rgb(102,194,165), rgb(252,141,98), rgb(141,160,203), rgb(231,138,195)}}},
      {5, BrewerPalette{Palette{rgb(102,194,165), rgb(252,141,98), rgb(141,160,203), rgb(231,138,195), rgb(166,216,84)}}},
      {6, BrewerPalette{Palette{rgb(102,194,165), rgb(252,141,98), rgb(141,160,203), rgb(231,138,195), rgb(166,216,84), rgb(255,217,47)}}},
      {7, BrewerPalette{Palette{rgb(102,194,165), rgb(252,141,98), rgb(141,160,203), rgb(231,138,195), rgb(166,216,84), rgb(255,217,47), rgb(229,196,148)}}},
      {8, BrewerPalette{Palette{rgb(102,194,165), rgb(252,141,98), rgb(141,160,203), rgb(231,138,195), rgb(166,216,84), rgb(255,217,47), rgb(229,196,148), rgb(179,179,179)}}},
    }, Type::qual, Properties{Blind{1,2,2,2,0,0,0},Print{1,1,1,2,2,2},Copy{0},Screen{1,1,2,2,2,2} } };
  return cb;
}

const ColorBrewer& Accent()  {
  static const ColorBrewer cb = ColorBrewer { "Accent",
    Map {
      {3, BrewerPalette{Palette{rgb(127,201,127), rgb(190,174,212), rgb(253,192,134)}}},
      {4, BrewerPalette{Palette{rgb(127,201,127), rgb(190,174,212), rgb(253,192,134), rgb(255,255,153)}}},
      {5, BrewerPalette{Palette{rgb(127,201,127), rgb(190,174,212), rgb(253,192,134), rgb(255,255,153), rgb(56,108,176)}}},
      {6, BrewerPalette{Palette{rgb(127,201,127), rgb(190,174,212), rgb(253,192,134), rgb(255,255,153), rgb(56,108,176), rgb(240,2,127)}}},
      {7, BrewerPalette{Palette{rgb(127,201,127), rgb(190,174,212), rgb(253,192,134), rgb(255,255,153), rgb(56,108,176), rgb(240,2,127), rgb(191,91,23)}}},
      {8, BrewerPalette{Palette{rgb(127,201,127), rgb(190,174,212), rgb(253,192,134), rgb(255,255,153), rgb(56,108,176), rgb(240,2,127), rgb(191,91,23), rgb(102,102,102)}}}
    }, Type::qual, Properties{Blind{2,0,0,0,0,0,0},Print{1,1,2,2,2,2},Copy{0},Screen{1,1,1,2,2,2} } };
  return cb;
}

const ColorBrewer& Set1()    {
  static const ColorBrewer cb = ColorBrewer { "Set1",
    Map {
      {3, BrewerPalette{Palette{rgb(228,26,28), rgb(55,126,184), rgb(77,175,74)}}},
      {4, BrewerPalette{Palette{rgb(228,26,28), rgb(55,126,184), rgb(77,175,74), rgb(152,78,163)}}},
      {5, BrewerPalette{Palette{rgb(228,26,28), rgb(55,126,184), rgb(77,175,74), rgb(152,78,163), rgb(255,127,0)}}},
      {6, BrewerPalette{Palette{rgb(228,26,28), rgb(55,126,184), rgb(77,175,74), rgb(152,78,163), rgb(255,127,0), rgb(255,255,51)}}},
      {7, BrewerPalette{Palette{rgb(228,26,28), rgb(55,126,184), rgb(77,175,74), rgb(152,78,163), rgb(255,127,0), rgb(255,255,51), rgb(166,86,40)}}},
      {8, BrewerPalette{Palette{rgb(228,26,28), rgb(55,126,184), rgb(77,175,74), rgb(152,78,163), rgb(255,127,0), rgb(255,255,51), rgb(166,86,40), rgb(247,129,191)}}},
      {9, BrewerPalette{Palette{rgb(228,26,28), rgb(55,126,184), rgb(77,175,74), rgb(152,78,163), rgb(255,127,0), rgb(255,255,51), rgb(166,86,40), rgb(247,129,191), rgb(153,153,153)}}}
    }, Type::qual, Properties{Blind{2},Print{1},Copy{0},Screen{1} } };
  return cb;
}

const ColorBrewer& Set3()    {
  static const ColorBrewer cb = ColorBrewer { "Set3",
    Map {
      {3,  BrewerPalette{Palette{rgb(141,211,199), rgb(255,255,179), rgb(190,186,218)}}},
      {4,  BrewerPalette{Palette{rgb(141,211,199), rgb(255,255,179), rgb(190,186,218), rgb(251,128,114)}}},
      {5,  BrewerPalette{Palette{rgb(141,211,199), rgb(255,255,179), rgb(190,186,218), rgb(251,128,114), rgb(128,177,211)}}},
      {6,  BrewerPalette{Palette{rgb(141,211,199), rgb(255,255,179), rgb(190,186,218), rgb(251,128,114), rgb(128,177,211), rgb(253,180,98)}}},
      {7,  BrewerPalette{Palette{rgb(141,211,199), rgb(255,255,179), rgb(190,186,218), rgb(251,128,114), rgb(128,177,211), rgb(253,180,98), rgb(179,222,105)}}},
      {8,  BrewerPalette{Palette{rgb(141,211,199), rgb(255,255,179), rgb(190,186,218), rgb(251,128,114), rgb(128,177,211), rgb(253,180,98), rgb(179,222,105), rgb(252,205,229)}}},
      {9,  BrewerPalette{Palette{rgb(141,211,199), rgb(255,255,179), rgb(190,186,218), rgb(251,128,114), rgb(128,177,211), rgb(253,180,98), rgb(179,222,105), rgb(252,205,229), rgb(217,217,217)}}},
      {10, BrewerPalette{Palette{rgb(141,211,199), rgb(255,255,179), rgb(190,186,218), rgb(251,128,114), rgb(128,177,211), rgb(253,180,98), rgb(179,222,105), rgb(252,205,229), rgb(217,217,217), rgb(188,128,189)}}},
      {11, BrewerPalette{Palette{rgb(141,211,199), rgb(255,255,179), rgb(190,186,218), rgb(251,128,114), rgb(128,177,211), rgb(253,180,98), rgb(179,222,105), rgb(252,205,229), rgb(217,217,217), rgb(188,128,189), rgb(204,235,197)}}},
      {12, BrewerPalette{Palette{rgb(141,211,199), rgb(255,255,179), rgb(190,186,218), rgb(251,128,114), rgb(128,177,211), rgb(253,180,98), rgb(179,222,105), rgb(252,205,229), rgb(217,217,217), rgb(188,128,189), rgb(204,235,197), rgb(255,237,111)}}}
    }, Type::qual, Properties{Blind{2,2,0,0,0,0,0,0,0,0},Print{1,1,1,1,1,1,2,0,0,0},Copy{1,2,2,2,2,2,2,0,0,0},Screen{1,1,1,2,2,2,0,0,0,0} } };
  return cb;
}

const ColorBrewer& Dark2()   {
  static const ColorBrewer cb = ColorBrewer { "Dark2",
    Map {
      {3, BrewerPalette{Palette{rgb(27,158,119), rgb(217,95,2), rgb(117,112,179)}}},
      {4, BrewerPalette{Palette{rgb(27,158,119), rgb(217,95,2), rgb(117,112,179), rgb(231,41,138)}}},
      {5, BrewerPalette{Palette{rgb(27,158,119), rgb(217,95,2), rgb(117,112,179), rgb(231,41,138), rgb(102,166,30)}}},
      {6, BrewerPalette{Palette{rgb(27,158,119), rgb(217,95,2), rgb(117,112,179), rgb(231,41,138), rgb(102,166,30), rgb(230,171,2)}}},
      {7, BrewerPalette{Palette{rgb(27,158,119), rgb(217,95,2), rgb(117,112,179), rgb(231,41,138), rgb(102,166,30), rgb(230,171,2), rgb(166,118,29)}}},
      {8, BrewerPalette{Palette{rgb(27,158,119), rgb(217,95,2), rgb(117,112,179), rgb(231,41,138), rgb(102,166,30), rgb(230,171,2), rgb(166,118,29), rgb(102,102,102)}}}
    }, Type::qual, Properties{Blind{1,2,2,2,0,0},Print{1},Copy{0},Screen{1} } };
  return cb;
}

const ColorBrewer& Paired()  {
  static const ColorBrewer cb = ColorBrewer { "Paired",
    Map {
      {3,  BrewerPalette{Palette{rgb(166,206,227), rgb(31,120,180), rgb(178,223,138)}}},
      {4,  BrewerPalette{Palette{rgb(166,206,227), rgb(31,120,180), rgb(178,223,138), rgb(51,160,44)}}},
      {5,  BrewerPalette{Palette{rgb(166,206,227), rgb(31,120,180), rgb(178,223,138), rgb(51,160,44), rgb(251,154,153)}}},
      {6,  BrewerPalette{Palette{rgb(166,206,227), rgb(31,120,180), rgb(178,223,138), rgb(51,160,44), rgb(251,154,153), rgb(227,26,28)}}},
      {7,  BrewerPalette{Palette{rgb(166,206,227), rgb(31,120,180), rgb(178,223,138), rgb(51,160,44), rgb(251,154,153), rgb(227,26,28), rgb(253,191,111)}}},
      {8,  BrewerPalette{Palette{rgb(166,206,227), rgb(31,120,180), rgb(178,223,138), rgb(51,160,44), rgb(251,154,153), rgb(227,26,28), rgb(253,191,111), rgb(255,127,0)}}},
      {9,  BrewerPalette{Palette{rgb(166,206,227), rgb(31,120,180), rgb(178,223,138), rgb(51,160,44), rgb(251,154,153), rgb(227,26,28), rgb(253,191,111), rgb(255,127,0), rgb(202,178,214)}}},
      {10, BrewerPalette{Palette{rgb(166,206,227), rgb(31,120,180), rgb(178,223,138), rgb(51,160,44), rgb(251,154,153), rgb(227,26,28), rgb(253,191,111), rgb(255,127,0), rgb(202,178,214), rgb(106,61,154)}}},
      {11, BrewerPalette{Palette{rgb(166,206,227), rgb(31,120,180), rgb(178,223,138), rgb(51,160,44), rgb(251,154,153), rgb(227,26,28), rgb(253,191,111), rgb(255,127,0), rgb(202,178,214), rgb(106,61,154), rgb(255,255,153)}}},
      {12, BrewerPalette{Palette{rgb(166,206,227), rgb(31,120,180), rgb(178,223,138), rgb(51,160,44), rgb(251,154,153), rgb(227,26,28), rgb(253,191,111), rgb(255,127,0), rgb(202,178,214), rgb(106,61,154), rgb(255,255,153), rgb(177,89,40)}}},
    }, Type::qual, Properties{Blind{1,1,2,2,2,2,0,0,0},Print{1,1,1,1,1,2,2,2,2},Copy{0},Screen{1,1,1,1,1,1,1,1,2} } };
  return cb;
}

const ColorBrewer& Pastel2() {
  static const ColorBrewer cb = ColorBrewer { "Pastel2",
    Map {
      {3, BrewerPalette{Palette{rgb(179,226,205), rgb(253,205,172), rgb(203,213,232)}}},
      {4, BrewerPalette{Palette{rgb(179,226,205), rgb(253,205,172), rgb(203,213,232), rgb(244,202,228)}}},
      {5, BrewerPalette{Palette{rgb(179,226,205), rgb(253,205,172), rgb(203,213,232), rgb(244,202,228), rgb(230,245,201)}}},
      {6, BrewerPalette{Palette{rgb(179,226,205), rgb(253,205,172), rgb(203,213,232), rgb(244,202,228), rgb(230,245,201), rgb(255,242,174)}}},
      {7, BrewerPalette{Palette{rgb(179,226,205), rgb(253,205,172), rgb(203,213,232), rgb(244,202,228), rgb(230,245,201), rgb(255,242,174), rgb(241,226,204)}}},
      {8, BrewerPalette{Palette{rgb(179,226,205), rgb(253,205,172), rgb(203,213,232), rgb(244,202,228), rgb(230,245,201), rgb(255,242,174), rgb(241,226,204), rgb(204,204,204)}}}
    }, Type::qual, Properties{Blind{2,0,0,0,0,0},Print{2,0,0,0,0,0},Copy{0},Screen{2,2,0,0,0,0} } };
  return cb;
}

const ColorBrewer& Pastel1() {
  static const ColorBrewer cb = ColorBrewer { "Pastel1",
    Map {
      {3, BrewerPalette{Palette{rgb(251,180,174), rgb(179,205,227), rgb(204,235,197)}}},
      {4, BrewerPalette{Palette{rgb(251,180,174), rgb(179,205,227), rgb(204,235,197), rgb(222,203,228)}}},
      {5, BrewerPalette{Palette{rgb(251,180,174), rgb(179,205,227), rgb(204,235,197), rgb(222,203,228), rgb(254,217,166)}}},
      {6, BrewerPalette{Palette{rgb(251,180,174), rgb(179,205,227), rgb(204,235,197), rgb(222,203,228), rgb(254,217,166), rgb(255,255,204)}}},
      {7, BrewerPalette{Palette{rgb(251,180,174), rgb(179,205,227), rgb(204,235,197), rgb(222,203,228), rgb(254,217,166), rgb(255,255,204), rgb(229,216,189)}}},
      {8, BrewerPalette{Palette{rgb(251,180,174), rgb(179,205,227), rgb(204,235,197), rgb(222,203,228), rgb(254,217,166), rgb(255,255,204), rgb(229,216,189), rgb(253,218,236)}}},
      {9, BrewerPalette{Palette{rgb(251,180,174), rgb(179,205,227), rgb(204,235,197), rgb(222,203,228), rgb(254,217,166), rgb(255,255,204), rgb(229,216,189), rgb(253,218,236), rgb(242,242,242)}}}
    }, Type::qual, Properties{Blind{2,0,0,0,0,0,0},Print{2,2,2,0,0,0,0},Copy{0},Screen{2,2,2,2,0,0,0} } };
  return cb;
}


/*** Sequential ***/
const ColorBrewer& OrRd()     {
  static const ColorBrewer cb = ColorBrewer { "OrRd",
    Map {
      {3, BrewerPalette{Palette{rgb(254,232,200), rgb(253,187,132), rgb(227,74,51)}}},
      {4, BrewerPalette{Palette{rgb(254,240,217), rgb(253,204,138), rgb(252,141,89), rgb(215,48,31)}}},
      {5, BrewerPalette{Palette{rgb(254,240,217), rgb(253,204,138), rgb(252,141,89), rgb(227,74,51), rgb(179,0,0)}}},
      {6, BrewerPalette{Palette{rgb(254,240,217), rgb(253,212,158), rgb(253,187,132), rgb(252,141,89), rgb(227,74,51), rgb(179,0,0)}}},
      {7, BrewerPalette{Palette{rgb(254,240,217), rgb(253,212,158), rgb(253,187,132), rgb(252,141,89), rgb(239,101,72), rgb(215,48,31), rgb(153,0,0)}}},
      {8, BrewerPalette{Palette{rgb(255,247,236), rgb(254,232,200), rgb(253,212,158), rgb(253,187,132), rgb(252,141,89), rgb(239,101,72), rgb(215,48,31), rgb(153,0,0)}}},
      {9, BrewerPalette{Palette{rgb(255,247,236), rgb(254,232,200), rgb(253,212,158), rgb(253,187,132), rgb(252,141,89), rgb(239,101,72), rgb(215,48,31), rgb(179,0,0), rgb(127,0,0)}}}
    }, Type::seq, Properties{Blind{1},Print{1,1,0,0,0,0,0},Copy{1,1,2,0,0,0,0},Screen{1,1,1,0,0,0,0} } };
  return cb;
}

const ColorBrewer& PuBu()     {
  static const ColorBrewer cb = ColorBrewer { "PuBu",
    Map {
      {3, BrewerPalette{Palette{rgb(236,231,242), rgb(166,189,219), rgb(43,140,190)}}},
      {4, BrewerPalette{Palette{rgb(241,238,246), rgb(189,201,225), rgb(116,169,207), rgb(5,112,176)}}},
      {5, BrewerPalette{Palette{rgb(241,238,246), rgb(189,201,225), rgb(116,169,207), rgb(43,140,190), rgb(4,90,141)}}},
      {6, BrewerPalette{Palette{rgb(241,238,246), rgb(208,209,230), rgb(166,189,219), rgb(116,169,207), rgb(43,140,190), rgb(4,90,141)}}},
      {7, BrewerPalette{Palette{rgb(241,238,246), rgb(208,209,230), rgb(166,189,219), rgb(116,169,207), rgb(54,144,192), rgb(5,112,176), rgb(3,78,123)}}},
      {8, BrewerPalette{Palette{rgb(255,247,251), rgb(236,231,242), rgb(208,209,230), rgb(166,189,219), rgb(116,169,207), rgb(54,144,192), rgb(5,112,176), rgb(3,78,123)}}},
      {9, BrewerPalette{Palette{rgb(255,247,251), rgb(236,231,242), rgb(208,209,230), rgb(166,189,219), rgb(116,169,207), rgb(54,144,192), rgb(5,112,176), rgb(4,90,141), rgb(2,56,88)}}}
    }, Type::seq, Properties{Blind{1},Print{1,2,2,0,0,0,0},Copy{1,2,0,0,0,0,0},Screen{1,1,2,0,0,0,0} } };
  return cb;
}

const ColorBrewer& BuPu()     {
  static const ColorBrewer cb = ColorBrewer { "BuPu",
    Map {
      {3, BrewerPalette{Palette{rgb(224,236,244), rgb(158,188,218), rgb(136,86,167)}}},
      {4, BrewerPalette{Palette{rgb(237,248,251), rgb(179,205,227), rgb(140,150,198), rgb(136,65,157)}}},
      {5, BrewerPalette{Palette{rgb(237,248,251), rgb(179,205,227), rgb(140,150,198), rgb(136,86,167), rgb(129,15,124)}}},
      {6, BrewerPalette{Palette{rgb(237,248,251), rgb(191,211,230), rgb(158,188,218), rgb(140,150,198), rgb(136,86,167), rgb(129,15,124)}}},
      {7, BrewerPalette{Palette{rgb(237,248,251), rgb(191,211,230), rgb(158,188,218), rgb(140,150,198), rgb(140,107,177), rgb(136,65,157), rgb(110,1,107)}}},
      {8, BrewerPalette{Palette{rgb(247,252,253), rgb(224,236,244), rgb(191,211,230), rgb(158,188,218), rgb(140,150,198), rgb(140,107,177), rgb(136,65,157), rgb(110,1,107)}}},
      {9, BrewerPalette{Palette{rgb(247,252,253), rgb(224,236,244), rgb(191,211,230), rgb(158,188,218), rgb(140,150,198), rgb(140,107,177), rgb(136,65,157), rgb(129,15,124), rgb(77,0,75)}}}
    }, Type::seq, Properties{Blind{1},Print{1,1,2,2,0,0,0},Copy{1,2,0,0,0,0,0},Screen{1,1,1,0,0,0,0} } };
  return cb;
}

const ColorBrewer& Oranges()  {
  static const ColorBrewer cb = ColorBrewer { "Oranges",
    Map {
      {3, BrewerPalette{Palette{rgb(254,230,206), rgb(253,174,107), rgb(230,85,13)}}},
      {4, BrewerPalette{Palette{rgb(254,237,222), rgb(253,190,133), rgb(253,141,60), rgb(217,71,1)}}},
      {5, BrewerPalette{Palette{rgb(254,237,222), rgb(253,190,133), rgb(253,141,60), rgb(230,85,13), rgb(166,54,3)}}},
      {6, BrewerPalette{Palette{rgb(254,237,222), rgb(253,208,162), rgb(253,174,107), rgb(253,141,60), rgb(230,85,13), rgb(166,54,3)}}},
      {7, BrewerPalette{Palette{rgb(254,237,222), rgb(253,208,162), rgb(253,174,107), rgb(253,141,60), rgb(241,105,19), rgb(217,72,1), rgb(140,45,4)}}},
      {8, BrewerPalette{Palette{rgb(255,245,235), rgb(254,230,206), rgb(253,208,162), rgb(253,174,107), rgb(253,141,60), rgb(241,105,19), rgb(217,72,1), rgb(140,45,4)}}},
      {9, BrewerPalette{Palette{rgb(255,245,235), rgb(254,230,206), rgb(253,208,162), rgb(253,174,107), rgb(253,141,60), rgb(241,105,19), rgb(217,72,1), rgb(166,54,3), rgb(127,39,4)}}}
    }, Type::seq, Properties{Blind{1},Print{1,2,0,0,0,0,0},Copy{1,2,2,0,0,0,0},Screen{1,1,1,0,0,0,0} } };
  return cb;
}

const ColorBrewer& BuGn()     {
  static const ColorBrewer cb = ColorBrewer { "BuGn",
    Map {
      {3, BrewerPalette{Palette{rgb(229,245,249), rgb(153,216,201), rgb(44,162,95)}}},
      {4, BrewerPalette{Palette{rgb(237,248,251), rgb(178,226,226), rgb(102,194,164), rgb(35,139,69)}}},
      {5, BrewerPalette{Palette{rgb(237,248,251), rgb(178,226,226), rgb(102,194,164), rgb(44,162,95), rgb(0,109,44)}}},
      {6, BrewerPalette{Palette{rgb(237,248,251), rgb(204,236,230), rgb(153,216,201), rgb(102,194,164), rgb(44,162,95), rgb(0,109,44)}}},
      {7, BrewerPalette{Palette{rgb(237,248,251), rgb(204,236,230), rgb(153,216,201), rgb(102,194,164), rgb(65,174,118), rgb(35,139,69), rgb(0,88,36)}}},
      {8, BrewerPalette{Palette{rgb(247,252,253), rgb(229,245,249), rgb(204,236,230), rgb(153,216,201), rgb(102,194,164), rgb(65,174,118), rgb(35,139,69), rgb(0,88,36)}}},
      {9, BrewerPalette{Palette{rgb(247,252,253), rgb(229,245,249), rgb(204,236,230), rgb(153,216,201), rgb(102,194,164), rgb(65,174,118), rgb(35,139,69), rgb(0,109,44), rgb(0,68,27)}}}
    }, Type::seq, Properties{Blind{1},Print{1,1,2,0,0,0,0},Copy{1,2,0,0,0,0,0},Screen{1,2,0,0,0,0,0} } };
  return cb;
}

const ColorBrewer& YlOrBr()   {
  static const ColorBrewer cb = ColorBrewer { "YlOrBr",
    Map {
      {3, BrewerPalette{Palette{rgb(255,247,188), rgb(254,196,79), rgb(217,95,14)}}},
      {4, BrewerPalette{Palette{rgb(255,255,212), rgb(254,217,142), rgb(254,153,41), rgb(204,76,2)}}},
      {5, BrewerPalette{Palette{rgb(255,255,212), rgb(254,217,142), rgb(254,153,41), rgb(217,95,14), rgb(153,52,4)}}},
      {6, BrewerPalette{Palette{rgb(255,255,212), rgb(254,227,145), rgb(254,196,79), rgb(254,153,41), rgb(217,95,14), rgb(153,52,4)}}},
      {7, BrewerPalette{Palette{rgb(255,255,212), rgb(254,227,145), rgb(254,196,79), rgb(254,153,41), rgb(236,112,20), rgb(204,76,2), rgb(140,45,4)}}},
      {8, BrewerPalette{Palette{rgb(255,255,229), rgb(255,247,188), rgb(254,227,145), rgb(254,196,79), rgb(254,153,41), rgb(236,112,20), rgb(204,76,2), rgb(140,45,4)}}},
      {9, BrewerPalette{Palette{rgb(255,255,229), rgb(255,247,188), rgb(254,227,145), rgb(254,196,79), rgb(254,153,41), rgb(236,112,20), rgb(204,76,2), rgb(153,52,4), rgb(102,37,6)}}}
    }, Type::seq, Properties{Blind{1},Print{1,1,2,0,0,0,0},Copy{1,2,2,0,0,0,0},Screen{1,2,0,0,0,0,0} } };
  return cb;
}

const ColorBrewer& YlGn()     {
  static const ColorBrewer cb = ColorBrewer { "YlGn",
    Map {
      {3, BrewerPalette{Palette{rgb(247,252,185), rgb(173,221,142), rgb(49,163,84)}}},
      {4, BrewerPalette{Palette{rgb(255,255,204), rgb(194,230,153), rgb(120,198,121), rgb(35,132,67)}}},
      {5, BrewerPalette{Palette{rgb(255,255,204), rgb(194,230,153), rgb(120,198,121), rgb(49,163,84), rgb(0,104,55)}}},
      {6, BrewerPalette{Palette{rgb(255,255,204), rgb(217,240,163), rgb(173,221,142), rgb(120,198,121), rgb(49,163,84), rgb(0,104,55)}}},
      {7, BrewerPalette{Palette{rgb(255,255,204), rgb(217,240,163), rgb(173,221,142), rgb(120,198,121), rgb(65,171,93), rgb(35,132,67), rgb(0,90,50)}}},
      {8, BrewerPalette{Palette{rgb(255,255,229), rgb(247,252,185), rgb(217,240,163), rgb(173,221,142), rgb(120,198,121), rgb(65,171,93), rgb(35,132,67), rgb(0,90,50)}}},
      {9, BrewerPalette{Palette{rgb(255,255,229), rgb(247,252,185), rgb(217,240,163), rgb(173,221,142), rgb(120,198,121), rgb(65,171,93), rgb(35,132,67), rgb(0,104,55), rgb(0,69,41)}}}
    }, Type::seq, Properties{Blind{1},Print{1,1,1,0,0,0,0},Copy{1,2,0,0,0,0,0},Screen{1,1,1,0,0,0,0} } };
  return cb;
}

const ColorBrewer& Reds()     {
  static const ColorBrewer cb = ColorBrewer { "Reds",
    Map {
      {3, BrewerPalette{Palette{rgb(254,224,210), rgb(252,146,114), rgb(222,45,38)}}},
      {4, BrewerPalette{Palette{rgb(254,229,217), rgb(252,174,145), rgb(251,106,74), rgb(203,24,29)}}},
      {5, BrewerPalette{Palette{rgb(254,229,217), rgb(252,174,145), rgb(251,106,74), rgb(222,45,38), rgb(165,15,21)}}},
      {6, BrewerPalette{Palette{rgb(254,229,217), rgb(252,187,161), rgb(252,146,114), rgb(251,106,74), rgb(222,45,38), rgb(165,15,21)}}},
      {7, BrewerPalette{Palette{rgb(254,229,217), rgb(252,187,161), rgb(252,146,114), rgb(251,106,74), rgb(239,59,44), rgb(203,24,29), rgb(153,0,13)}}},
      {8, BrewerPalette{Palette{rgb(255,245,240), rgb(254,224,210), rgb(252,187,161), rgb(252,146,114), rgb(251,106,74), rgb(239,59,44), rgb(203,24,29), rgb(153,0,13)}}},
      {9, BrewerPalette{Palette{rgb(255,245,240), rgb(254,224,210), rgb(252,187,161), rgb(252,146,114), rgb(251,106,74), rgb(239,59,44), rgb(203,24,29), rgb(165,15,21), rgb(103,0,13)}}}
    }, Type::seq, Properties{Blind{1},Print{1,2,2,0,0,0,0},Copy{1,2,0,0,0,0,0},Screen{1,2,0,0,0,0,0} } };
  return cb;
}

const ColorBrewer& RdPu()     {
  static const ColorBrewer cb = ColorBrewer { "RdPu",
    Map {
      {3, BrewerPalette{Palette{rgb(253,224,221), rgb(250,159,181), rgb(197,27,138)}}},
      {4, BrewerPalette{Palette{rgb(254,235,226), rgb(251,180,185), rgb(247,104,161), rgb(174,1,126)}}},
      {5, BrewerPalette{Palette{rgb(254,235,226), rgb(251,180,185), rgb(247,104,161), rgb(197,27,138), rgb(122,1,119)}}},
      {6, BrewerPalette{Palette{rgb(254,235,226), rgb(252,197,192), rgb(250,159,181), rgb(247,104,161), rgb(197,27,138), rgb(122,1,119)}}},
      {7, BrewerPalette{Palette{rgb(254,235,226), rgb(252,197,192), rgb(250,159,181), rgb(247,104,161), rgb(221,52,151), rgb(174,1,126), rgb(122,1,119)}}},
      {8, BrewerPalette{Palette{rgb(255,247,243), rgb(253,224,221), rgb(252,197,192), rgb(250,159,181), rgb(247,104,161), rgb(221,52,151), rgb(174,1,126), rgb(122,1,119)}}},
      {9, BrewerPalette{Palette{rgb(255,247,243), rgb(253,224,221), rgb(252,197,192), rgb(250,159,181), rgb(247,104,161), rgb(221,52,151), rgb(174,1,126), rgb(122,1,119), rgb(73,0,106)}}}
    }, Type::seq, Properties{Blind{1},Print{1,1,1,2,0,0,0},Copy{1,2,0,0,0,0,0},Screen{1,1,1,0,0,0,0} } };
  return cb;
}

const ColorBrewer& Greens()   {
  static const ColorBrewer cb = ColorBrewer { "Greens",
    Map {
      {3, BrewerPalette{Palette{rgb(229,245,224), rgb(161,217,155), rgb(49,163,84)}}},
      {4, BrewerPalette{Palette{rgb(237,248,233), rgb(186,228,179), rgb(116,196,118), rgb(35,139,69)}}},
      {5, BrewerPalette{Palette{rgb(237,248,233), rgb(186,228,179), rgb(116,196,118), rgb(49,163,84), rgb(0,109,44)}}},
      {6, BrewerPalette{Palette{rgb(237,248,233), rgb(199,233,192), rgb(161,217,155), rgb(116,196,118), rgb(49,163,84), rgb(0,109,44)}}},
      {7, BrewerPalette{Palette{rgb(237,248,233), rgb(199,233,192), rgb(161,217,155), rgb(116,196,118), rgb(65,171,93), rgb(35,139,69), rgb(0,90,50)}}},
      {8, BrewerPalette{Palette{rgb(247,252,245), rgb(229,245,224), rgb(199,233,192), rgb(161,217,155), rgb(116,196,118), rgb(65,171,93), rgb(35,139,69), rgb(0,90,50)}}},
      {9, BrewerPalette{Palette{rgb(247,252,245), rgb(229,245,224), rgb(199,233,192), rgb(161,217,155), rgb(116,196,118), rgb(65,171,93), rgb(35,139,69), rgb(0,109,44), rgb(0,68,27)}}}
    }, Type::seq, Properties{Blind{1},Print{1,0,0,0,0,0,0},Copy{1,2,0,0,0,0,0},Screen{1,2,0,0,0,0,0} } };
  return cb;
}

const ColorBrewer& YlGnBu()   {
  static const ColorBrewer cb = ColorBrewer { "YlGnBu",
    Map {
      {3, BrewerPalette{Palette{rgb(237,248,177), rgb(127,205,187), rgb(44,127,184)}}},
      {4, BrewerPalette{Palette{rgb(255,255,204), rgb(161,218,180), rgb(65,182,196), rgb(34,94,168)}}},
      {5, BrewerPalette{Palette{rgb(255,255,204), rgb(161,218,180), rgb(65,182,196), rgb(44,127,184), rgb(37,52,148)}}},
      {6, BrewerPalette{Palette{rgb(255,255,204), rgb(199,233,180), rgb(127,205,187), rgb(65,182,196), rgb(44,127,184), rgb(37,52,148)}}},
      {7, BrewerPalette{Palette{rgb(255,255,204), rgb(199,233,180), rgb(127,205,187), rgb(65,182,196), rgb(29,145,192), rgb(34,94,168), rgb(12,44,132)}}},
      {8, BrewerPalette{Palette{rgb(255,255,217), rgb(237,248,177), rgb(199,233,180), rgb(127,205,187), rgb(65,182,196), rgb(29,145,192), rgb(34,94,168), rgb(12,44,132)}}},
      {9, BrewerPalette{Palette{rgb(255,255,217), rgb(237,248,177), rgb(199,233,180), rgb(127,205,187), rgb(65,182,196), rgb(29,145,192), rgb(34,94,168), rgb(37,52,148), rgb(8,29,88)}}}
    }, Type::seq, Properties{Blind{1},Print{1,1,1,2,2,2,0},Copy{1,2,0,0,0,0,0},Screen{1,1,2,0,0,0,0} } };
  return cb;
}

const ColorBrewer& Purples()  {
  static const ColorBrewer cb = ColorBrewer { "Purples",
    Map {
      {3, BrewerPalette{Palette{rgb(239,237,245), rgb(188,189,220), rgb(117,107,177)}}},
      {4, BrewerPalette{Palette{rgb(242,240,247), rgb(203,201,226), rgb(158,154,200), rgb(106,81,163)}}},
      {5, BrewerPalette{Palette{rgb(242,240,247), rgb(203,201,226), rgb(158,154,200), rgb(117,107,177), rgb(84,39,143)}}},
      {6, BrewerPalette{Palette{rgb(242,240,247), rgb(218,218,235), rgb(188,189,220), rgb(158,154,200), rgb(117,107,177), rgb(84,39,143)}}},
      {7, BrewerPalette{Palette{rgb(242,240,247), rgb(218,218,235), rgb(188,189,220), rgb(158,154,200), rgb(128,125,186), rgb(106,81,163), rgb(74,20,134)}}},
      {8, BrewerPalette{Palette{rgb(252,251,253), rgb(239,237,245), rgb(218,218,235), rgb(188,189,220), rgb(158,154,200), rgb(128,125,186), rgb(106,81,163), rgb(74,20,134)}}},
      {9, BrewerPalette{Palette{rgb(252,251,253), rgb(239,237,245), rgb(218,218,235), rgb(188,189,220), rgb(158,154,200), rgb(128,125,186), rgb(106,81,163), rgb(84,39,143), rgb(63,0,125)}}}
    }, Type::seq, Properties{Blind{1},Print{1,0,0,0,0,0,0},Copy{1,2,0,0,0,0,0},Screen{1,0,0,0,0,0,0} } };
  return cb;
}

const ColorBrewer& GnBu()     {
  static const ColorBrewer cb = ColorBrewer { "GnBu",
    Map {
      {3, BrewerPalette{Palette{rgb(224,243,219), rgb(168,221,181), rgb(67,162,202)}}},
      {4, BrewerPalette{Palette{rgb(240,249,232), rgb(186,228,188), rgb(123,204,196), rgb(43,140,190)}}},
      {5, BrewerPalette{Palette{rgb(240,249,232), rgb(186,228,188), rgb(123,204,196), rgb(67,162,202), rgb(8,104,172)}}},
      {6, BrewerPalette{Palette{rgb(240,249,232), rgb(204,235,197), rgb(168,221,181), rgb(123,204,196), rgb(67,162,202), rgb(8,104,172)}}},
      {7, BrewerPalette{Palette{rgb(240,249,232), rgb(204,235,197), rgb(168,221,181), rgb(123,204,196), rgb(78,179,211), rgb(43,140,190), rgb(8,88,158)}}},
      {8, BrewerPalette{Palette{rgb(247,252,240), rgb(224,243,219), rgb(204,235,197), rgb(168,221,181), rgb(123,204,196), rgb(78,179,211), rgb(43,140,190), rgb(8,88,158)}}},
      {9, BrewerPalette{Palette{rgb(247,252,240), rgb(224,243,219), rgb(204,235,197), rgb(168,221,181), rgb(123,204,196), rgb(78,179,211), rgb(43,140,190), rgb(8,104,172), rgb(8,64,129)}}}
    }, Type::seq, Properties{Blind{1},Print{1,1,1,2,2,2,0},Copy{1,2,0,0,0,0,0},Screen{1,1,2,0,0,0,0} } };
  return cb;
}

const ColorBrewer& Greys()    {
  static const ColorBrewer cb = ColorBrewer { "Greys",
    Map {
      {3, BrewerPalette{Palette{rgb(240,240,240), rgb(189,189,189), rgb(99,99,99)}}},
      {4, BrewerPalette{Palette{rgb(247,247,247), rgb(204,204,204), rgb(150,150,150), rgb(82,82,82)}}},
      {5, BrewerPalette{Palette{rgb(247,247,247), rgb(204,204,204), rgb(150,150,150), rgb(99,99,99), rgb(37,37,37)}}},
      {6, BrewerPalette{Palette{rgb(247,247,247), rgb(217,217,217), rgb(189,189,189), rgb(150,150,150), rgb(99,99,99), rgb(37,37,37)}}},
      {7, BrewerPalette{Palette{rgb(247,247,247), rgb(217,217,217), rgb(189,189,189), rgb(150,150,150), rgb(115,115,115), rgb(82,82,82), rgb(37,37,37)}}},
      {8, BrewerPalette{Palette{rgb(255,255,255), rgb(240,240,240), rgb(217,217,217), rgb(189,189,189), rgb(150,150,150), rgb(115,115,115), rgb(82,82,82), rgb(37,37,37)}}},
      {9, BrewerPalette{Palette{rgb(255,255,255), rgb(240,240,240), rgb(217,217,217), rgb(189,189,189), rgb(150,150,150), rgb(115,115,115), rgb(82,82,82), rgb(37,37,37), rgb(0,0,0)}}}
    }, Type::seq, Properties{Blind{1},Print{1,1,2,0,0,0,0},Copy{1,0,0,0,0,0,0},Screen{1,2,0,0,0,0,0} } };
  return cb;
}

const ColorBrewer& YlOrRd()   {
  static const ColorBrewer cb = ColorBrewer { "YlOrRd",
    Map {
      {3, BrewerPalette{Palette{rgb(255,237,160), rgb(254,178,76), rgb(240,59,32)}}},
      {4, BrewerPalette{Palette{rgb(255,255,178), rgb(254,204,92), rgb(253,141,60), rgb(227,26,28)}}},
      {5, BrewerPalette{Palette{rgb(255,255,178), rgb(254,204,92), rgb(253,141,60), rgb(240,59,32), rgb(189,0,38)}}},
      {6, BrewerPalette{Palette{rgb(255,255,178), rgb(254,217,118), rgb(254,178,76), rgb(253,141,60), rgb(240,59,32), rgb(189,0,38)}}},
      {7, BrewerPalette{Palette{rgb(255,255,178), rgb(254,217,118), rgb(254,178,76), rgb(253,141,60), rgb(252,78,42), rgb(227,26,28), rgb(177,0,38)}}},
      {8, BrewerPalette{Palette{rgb(255,255,204), rgb(255,237,160), rgb(254,217,118), rgb(254,178,76), rgb(253,141,60), rgb(252,78,42), rgb(227,26,28), rgb(177,0,38)}}},
      {9, BrewerPalette{Palette{rgb(255,255,204),rgb(255,237,160),rgb(254,217,118),rgb(254,178,76),rgb(253,141,60),rgb(252,78,42),rgb(227,26,28),rgb(189,0,38),rgb(128,0,38)}}}
    }, Type::seq, Properties{Blind{1},Print{1,1,2,2,0,0,0},Copy{1,2,2,0,0,0,0},Screen{1,2,2,0,0,0,0} } };
  return cb;
}

const ColorBrewer& PuRd()     {
  static const ColorBrewer cb = ColorBrewer { "PuRd",
    Map {
      {3, BrewerPalette{Palette{rgb(231,225,239), rgb(201,148,199), rgb(221,28,119)}}},
      {4, BrewerPalette{Palette{rgb(241,238,246), rgb(215,181,216), rgb(223,101,176), rgb(206,18,86)}}},
      {5, BrewerPalette{Palette{rgb(241,238,246), rgb(215,181,216), rgb(223,101,176), rgb(221,28,119), rgb(152,0,67)}}},
      {6, BrewerPalette{Palette{rgb(241,238,246), rgb(212,185,218), rgb(201,148,199), rgb(223,101,176), rgb(221,28,119), rgb(152,0,67)}}},
      {7, BrewerPalette{Palette{rgb(241,238,246), rgb(212,185,218), rgb(201,148,199), rgb(223,101,176), rgb(231,41,138), rgb(206,18,86), rgb(145,0,63)}}},
      {8, BrewerPalette{Palette{rgb(247,244,249), rgb(231,225,239), rgb(212,185,218), rgb(201,148,199), rgb(223,101,176), rgb(231,41,138), rgb(206,18,86), rgb(145,0,63)}}},
      {9, BrewerPalette{Palette{rgb(247,244,249), rgb(231,225,239), rgb(212,185,218), rgb(201,148,199), rgb(223,101,176), rgb(231,41,138), rgb(206,18,86), rgb(152,0,67), rgb(103,0,31)}}}
    }, Type::seq, Properties{Blind{1},Print{1,1,1,0,0,0,0},Copy{1,2,0,0,0,0,0},Screen{1,1,1,0,0,0,0} } };
  return cb;
}

const ColorBrewer& Blues()    {
  static const ColorBrewer cb = ColorBrewer { "Blues",
    Map {
      {3, BrewerPalette{Palette{rgb(222,235,247), rgb(158,202,225), rgb(49,130,189)}}},
      {4, BrewerPalette{Palette{rgb(239,243,255), rgb(189,215,231), rgb(107,174,214), rgb(33,113,181)}}},
      {5, BrewerPalette{Palette{rgb(239,243,255), rgb(189,215,231), rgb(107,174,214), rgb(49,130,189), rgb(8,81,156)}}},
      {6, BrewerPalette{Palette{rgb(239,243,255), rgb(198,219,239), rgb(158,202,225), rgb(107,174,214), rgb(49,130,189), rgb(8,81,156)}}},
      {7, BrewerPalette{Palette{rgb(239,243,255), rgb(198,219,239), rgb(158,202,225), rgb(107,174,214), rgb(66,146,198), rgb(33,113,181), rgb(8,69,148)}}},
      {8, BrewerPalette{Palette{rgb(247,251,255), rgb(222,235,247), rgb(198,219,239), rgb(158,202,225), rgb(107,174,214), rgb(66,146,198), rgb(33,113,181), rgb(8,69,148)}}},
      {9, BrewerPalette{Palette{rgb(247,251,255), rgb(222,235,247), rgb(198,219,239), rgb(158,202,225), rgb(107,174,214), rgb(66,146,198), rgb(33,113,181), rgb(8,81,156), rgb(8,48,107)}}}
    }, Type::seq, Properties{Blind{1},Print{1,2,0,0,0,0,0},Copy{1,0,0,0,0,0,0},Screen{1,2,0,0,0,0,0} } };
  return cb;
}

const ColorBrewer& PuBuGn()   {
  static const ColorBrewer cb = ColorBrewer { "PuBuGn",
    Map {
      {3, BrewerPalette{Palette{rgb(236,226,240), rgb(166,189,219), rgb(28,144,153)}}},
      {4, BrewerPalette{Palette{rgb(246,239,247), rgb(189,201,225), rgb(103,169,207), rgb(2,129,138)}}},
      {5, BrewerPalette{Palette{rgb(246,239,247), rgb(189,201,225), rgb(103,169,207), rgb(28,144,153), rgb(1,108,89)}}},
      {6, BrewerPalette{Palette{rgb(246,239,247), rgb(208,209,230), rgb(166,189,219), rgb(103,169,207), rgb(28,144,153), rgb(1,108,89)}}},
      {7, BrewerPalette{Palette{rgb(246,239,247), rgb(208,209,230), rgb(166,189,219), rgb(103,169,207), rgb(54,144,192), rgb(2,129,138), rgb(1,100,80)}}},
      {8, BrewerPalette{Palette{rgb(255,247,251), rgb(236,226,240), rgb(208,209,230), rgb(166,189,219), rgb(103,169,207), rgb(54,144,192), rgb(2,129,138), rgb(1,100,80)}}},
      {9, BrewerPalette{Palette{rgb(255,247,251), rgb(236,226,240), rgb(208,209,230), rgb(166,189,219), rgb(103,169,207), rgb(54,144,192), rgb(2,129,138), rgb(1,108,89), rgb(1,70,54)}}}
    }, Type::seq, Properties{Blind{1},Print{1,2,2,0,0,0,0},Copy{1,2,0,0,0,0,0},Screen{1,1,2,0,0,0,0} } };
    return cb;
  }



}
