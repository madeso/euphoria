#include "core/colors.h"

#include <map>
#include <string>
#include <iostream>

namespace
{
  std::map<std::string, Color>
  CreateStringToColorMap()
  {
    std::map<std::string, Color> map;
#define ADD(name, value) map[name] = Color::value
    ADD("white", White);
    ADD("silver", Silver);
    ADD("gray", Gray);
    ADD("black", Black);
    ADD("navy", Navy);
    ADD("blue", Blue);
    ADD("aqua", Aqua);
    ADD("teal", Teal);
    ADD("green", Green);
    ADD("olive", Olive);
    ADD("lime", Lime);
    ADD("maroon", Maroon);
    ADD("red", Red);
    ADD("orange", Orange);
    ADD("yellow", Yellow);
    ADD("purple", Purple);
    ADD("fuchsia", Fuchsia);
    ADD("aliceblue", AliceBlue);
    ADD("antiquewhite", AntiqueWhite);
    ADD("aquamarine", AquaMarine);
    ADD("azure", Azure);
    ADD("beige", Beige);
    ADD("bisque", Bisque);
    ADD("blanchedalmond", BlanchedAlmond);
    ADD("blueviolet", BlueViolet);
    ADD("brown", Brown);
    ADD("burlywood", BurlyWood);
    ADD("cadetblue", CadetBlue);
    ADD("chartreuse", Chartreuse);
    ADD("chocolate", Chocolate);
    ADD("coral", Coral);
    ADD("cornflowerblue", CornflowerBlue);
    ADD("cornsilk", Cornsilk);
    ADD("crimson", Crimson);
    ADD("cyan", Cyan);
    ADD("darkblue", Darkblue);
    ADD("darkcyan", Darkcyan);
    ADD("darkgoldenrod", DarkGoldenRod);
    ADD("darkgray", Darkgray);
    ADD("darkgreen", Darkgreen);
    ADD("darkkhaki", DarKkhaki);
    ADD("darkmagenta", DarkMagenta);
    ADD("darkolivegreen", DarkOliveGreen);
    ADD("darkorange", DarkOrange);
    ADD("darkorchid", DarkOrchid);
    ADD("darkred", DarkRed);
    ADD("darksalmon", DarkSalmon);
    ADD("darkseagreen", DarkSeaGreen);
    ADD("darkslateblue", DarkslateBlue);
    ADD("darkslategray", DarkslateGray);
    ADD("darkturquoise", DarkTurquoise);
    ADD("darkbiolet", DarkBiolet);
    ADD("deeppink", DeeppInk);
    ADD("deepskyblue", DeepskyBlue);
    ADD("dimgray", DimGray);
    ADD("dodgerblue", DodgerBlue);
    ADD("firebrick", FireBrick);
    ADD("floralwhite", FloralWhite);
    ADD("forestgreen", ForestGreen);
    ADD("gainsboro", Gainsboro);
    ADD("ghostwhite", GhostWhite);
    ADD("gold", Gold);
    ADD("goldenrod", GoldenRod);
    ADD("greenyellow", GreenYellow);
    ADD("honeydew", Honeydew);
    ADD("hotpink", Hotpink);
    ADD("indianred", IndianRed);
    ADD("indigo", Indigo);
    ADD("ivory", Ivory);
    ADD("khaki", Khaki);
    ADD("lavender", Lavender);
    ADD("lavenderblush", LavenderBlush);
    ADD("lawngreen", Lawngreen);
    ADD("lemonchiffon", LemonChiffon);
    ADD("lightblue", LightBlue);
    ADD("lightcoral", LightCoral);
    ADD("lightcyan", LightCyan);
    ADD("lightgoldenrodyellow", LightGoldenRodYellow);
    ADD("lightgray", LightGray);
    ADD("lightgreen", LightGreen);
    ADD("lightpink", LightPink);
    ADD("lightsalmon", LightSalmon);
    ADD("lightseagreen", LightSeaGreen);
    ADD("lightskyblue", LightskyBlue);
    ADD("lightslategray", LightslateGray);
    ADD("lightsteelblue", LightSteelBlue);
    ADD("lightyellow", LightYellow);
    ADD("limegreen", LimeGreen);
    ADD("linen", Linen);
    ADD("magenta", Magenta);
    ADD("mediumaquamarine", MediumAquaMarine);
    ADD("mediumblue", MediumBlue);
    ADD("mediumorchid", MediumOrchid);
    ADD("mediumpurple", MediumPurple);
    ADD("mediumseagreen", MediumSeaGreen);
    ADD("mediumslateblue", MediumslateBlue);
    ADD("mediumspringgreen", MediumSpringGreen);
    ADD("mediumturquoise", MediumTurquoise);
    ADD("mediumvioletred", MediumVioletRed);
    ADD("midnightblue", MidnightBlue);
    ADD("mintcream", Mintcream);
    ADD("mistyrose", Mistyrose);
    ADD("moccasin", Moccasin);
    ADD("navajowhite", NavajoWhite);
    ADD("oldlace", Oldlace);
    ADD("olivedrab", OliveDrab);
    ADD("orangered", OrangeRed);
    ADD("orchid", Orchid);
    ADD("palegoldenrod", PaleGoldenRod);
    ADD("palegreen", PaleGreen);
    ADD("paleturquoise", PaleTurquoise);
    ADD("palevioletred", PaleVioletRed);
    ADD("papayawhip", Papayawhip);
    ADD("peachpuff", Peachpuff);
    ADD("peru", Peru);
    ADD("pink", Pink);
    ADD("plum", Plum);
    ADD("powderblue", PowderBlue);
    ADD("rosybrown", Rosybrown);
    ADD("royalblue", Royalblue);
    ADD("saddlebrown", SaddleBrown);
    ADD("salmon", Salmon);
    ADD("sandybrown", SandyBrown);
    ADD("seagreen", Seagreen);
    ADD("seashell", Seashell);
    ADD("sienna", Sienna);
    ADD("skyblue", SkyBlue);
    ADD("slateblue", SlateBlue);
    ADD("slategray", SlateGray);
    ADD("snow", Snow);
    ADD("springgreen", SpringGreen);
    ADD("steelblue", SteelBlue);
    ADD("tan", Tan);
    ADD("thistle", Thistle);
    ADD("tomato", Tomato);
    ADD("turquoise", Turquoise);
    ADD("violet", Violet);
    ADD("wheat", Wheat);
    ADD("whitesmoke", WhiteSmoke);
    ADD("yellowgreen", YellowGreen);
#undef ADD
    return map;
  }

  const std::map<std::string, Color>&
  StaticStringToColorMap()
  {
    static const auto Map = CreateStringToColorMap();
    return Map;
  }
}

namespace color
{
  bool
  IsValidLowerCase(const std::string& name)
  {
    const auto& map = StaticStringToColorMap();
    const auto  r   = map.find(name);
    return r != map.end();
  }

  Color
  GetColorFromLowerCaseString(const std::string& name)
  {
    const auto& map = StaticStringToColorMap();
    const auto  r   = map.find(name);
    if(r == map.end())
    {
      return Color::Black;
    }
    return r->second;
  }

  std::vector<std::string>
  ListAllColorNames()
  {
    const auto&              map = StaticStringToColorMap();
    std::vector<std::string> r;
    for(const auto& c : map)
    {
      r.emplace_back(c.first);
    }
    return r;
  }
}

#define HEXCODE(x) x

unsigned int
ToColorHex(Color color)
{
  switch(color)
  {
    case Color::White:
      return HEXCODE(0xffffff);
    case Color::Silver:
      return HEXCODE(0xc0c0c0);
    case Color::Gray:
      return HEXCODE(0x808080);
    case Color::Black:
      return HEXCODE(0x000000);
    case Color::Navy:
      return HEXCODE(0x000080);
    case Color::Blue:
      return HEXCODE(0x0000ff);
    case Color::Aqua:
      return HEXCODE(0x00ffff);
    case Color::Teal:
      return HEXCODE(0x008080);
    case Color::Green:
      return HEXCODE(0x008000);
    case Color::Olive:
      return HEXCODE(0x808000);
    case Color::Lime:
      return HEXCODE(0x00ff00);
    case Color::Maroon:
      return HEXCODE(0x800000);
    case Color::Red:
      return HEXCODE(0xff0000);
    case Color::Orange:
      return HEXCODE(0xffa500);
    case Color::Yellow:
      return HEXCODE(0xffff00);
    case Color::Purple:
      return HEXCODE(0x800080);
    case Color::Fuchsia:
      return HEXCODE(0xff00ff);
    case Color::AliceBlue:
      return HEXCODE(0xf0f8ff);
    case Color::AntiqueWhite:
      return HEXCODE(0xfaebd7);
    case Color::AquaMarine:
      return HEXCODE(0x7fffd4);
    case Color::Azure:
      return HEXCODE(0xf0ffff);
    case Color::Beige:
      return HEXCODE(0xf5f5dc);
    case Color::Bisque:
      return HEXCODE(0xffe4c4);
    case Color::BlanchedAlmond:
      return HEXCODE(0xffebcd);
    case Color::BlueViolet:
      return HEXCODE(0x8a2be2);
    case Color::Brown:
      return HEXCODE(0xa52a2a);
    case Color::BurlyWood:
      return HEXCODE(0xdeb887);
    case Color::CadetBlue:
      return HEXCODE(0x5f9ea0);
    case Color::Chartreuse:
      return HEXCODE(0x7fff00);
    case Color::Chocolate:
      return HEXCODE(0xd2691e);
    case Color::Coral:
      return HEXCODE(0xff7f50);
    case Color::CornflowerBlue:
      return HEXCODE(0x6495ed);
    case Color::Cornsilk:
      return HEXCODE(0xfff8dc);
    case Color::Crimson:
      return HEXCODE(0xdc143c);
    case Color::Cyan:
      return HEXCODE(0x00ffff);
    case Color::Darkblue:
      return HEXCODE(0x00008b);
    case Color::Darkcyan:
      return HEXCODE(0x008b8b);
    case Color::DarkGoldenRod:
      return HEXCODE(0xb8860b);
    case Color::Darkgray:
      return HEXCODE(0xa9a9a9);
    case Color::Darkgreen:
      return HEXCODE(0x006400);
    case Color::DarKkhaki:
      return HEXCODE(0xbdb76b);
    case Color::DarkMagenta:
      return HEXCODE(0x8b008b);
    case Color::DarkOliveGreen:
      return HEXCODE(0x556b2f);
    case Color::DarkOrange:
      return HEXCODE(0xff8c00);
    case Color::DarkOrchid:
      return HEXCODE(0x9932cc);
    case Color::DarkRed:
      return HEXCODE(0x8b0000);
    case Color::DarkSalmon:
      return HEXCODE(0xe9967a);
    case Color::DarkSeaGreen:
      return HEXCODE(0x8fbc8f);
    case Color::DarkslateBlue:
      return HEXCODE(0x483d8b);
    case Color::DarkslateGray:
      return HEXCODE(0x2f4f4f);
    case Color::DarkTurquoise:
      return HEXCODE(0x00ced1);
    case Color::DarkBiolet:
      return HEXCODE(0x9400d3);
    case Color::DeeppInk:
      return HEXCODE(0xff1493);
    case Color::DeepskyBlue:
      return HEXCODE(0x00bfff);
    case Color::DimGray:
      return HEXCODE(0x696969);
    case Color::DodgerBlue:
      return HEXCODE(0x1e90ff);
    case Color::FireBrick:
      return HEXCODE(0xb22222);
    case Color::FloralWhite:
      return HEXCODE(0xfffaf0);
    case Color::ForestGreen:
      return HEXCODE(0x228b22);
    case Color::Gainsboro:
      return HEXCODE(0xdcdcdc);
    case Color::GhostWhite:
      return HEXCODE(0xf8f8ff);
    case Color::Gold:
      return HEXCODE(0xffd700);
    case Color::GoldenRod:
      return HEXCODE(0xdaa520);
    case Color::GreenYellow:
      return HEXCODE(0xadff2f);
    case Color::Honeydew:
      return HEXCODE(0xf0fff0);
    case Color::Hotpink:
      return HEXCODE(0xff69b4);
    case Color::IndianRed:
      return HEXCODE(0xcd5c5c);
    case Color::Indigo:
      return HEXCODE(0x4b0082);
    case Color::Ivory:
      return HEXCODE(0xfffff0);
    case Color::Khaki:
      return HEXCODE(0xf0e68c);
    case Color::Lavender:
      return HEXCODE(0xe6e6fa);
    case Color::LavenderBlush:
      return HEXCODE(0xfff0f5);
    case Color::Lawngreen:
      return HEXCODE(0x7cfc00);
    case Color::LemonChiffon:
      return HEXCODE(0xfffacd);
    case Color::LightBlue:
      return HEXCODE(0xadd8e6);
    case Color::LightCoral:
      return HEXCODE(0xf08080);
    case Color::LightCyan:
      return HEXCODE(0xe0ffff);
    case Color::LightGoldenRodYellow:
      return HEXCODE(0xfafad2);
    case Color::LightGray:
      return HEXCODE(0xd3d3d3);
    case Color::LightGreen:
      return HEXCODE(0x90ee90);
    case Color::LightPink:
      return HEXCODE(0xffb6c1);
    case Color::LightSalmon:
      return HEXCODE(0xffa07a);
    case Color::LightSeaGreen:
      return HEXCODE(0x20b2aa);
    case Color::LightskyBlue:
      return HEXCODE(0x87cefa);
    case Color::LightslateGray:
      return HEXCODE(0x778899);
    case Color::LightSteelBlue:
      return HEXCODE(0xb0c4de);
    case Color::LightYellow:
      return HEXCODE(0xffffe0);
    case Color::LimeGreen:
      return HEXCODE(0x32cd32);
    case Color::Linen:
      return HEXCODE(0xfaf0e6);
    case Color::Magenta:
      return HEXCODE(0xff00ff);
    case Color::MediumAquaMarine:
      return HEXCODE(0x66cdaa);
    case Color::MediumBlue:
      return HEXCODE(0x0000cd);
    case Color::MediumOrchid:
      return HEXCODE(0xba55d3);
    case Color::MediumPurple:
      return HEXCODE(0x9370db);
    case Color::MediumSeaGreen:
      return HEXCODE(0x3cb371);
    case Color::MediumslateBlue:
      return HEXCODE(0x7b68ee);
    case Color::MediumSpringGreen:
      return HEXCODE(0x00fa9a);
    case Color::MediumTurquoise:
      return HEXCODE(0x48d1cc);
    case Color::MediumVioletRed:
      return HEXCODE(0xc71585);
    case Color::MidnightBlue:
      return HEXCODE(0x191970);
    case Color::Mintcream:
      return HEXCODE(0xf5fffa);
    case Color::Mistyrose:
      return HEXCODE(0xffe4e1);
    case Color::Moccasin:
      return HEXCODE(0xffe4b5);
    case Color::NavajoWhite:
      return HEXCODE(0xffdead);
    case Color::Oldlace:
      return HEXCODE(0xfdf5e6);
    case Color::OliveDrab:
      return HEXCODE(0x6b8e23);
    case Color::OrangeRed:
      return HEXCODE(0xff4500);
    case Color::Orchid:
      return HEXCODE(0xda70d6);
    case Color::PaleGoldenRod:
      return HEXCODE(0xeee8aa);
    case Color::PaleGreen:
      return HEXCODE(0x98fb98);
    case Color::PaleTurquoise:
      return HEXCODE(0xafeeee);
    case Color::PaleVioletRed:
      return HEXCODE(0xdb7093);
    case Color::Papayawhip:
      return HEXCODE(0xffefd5);
    case Color::Peachpuff:
      return HEXCODE(0xffdab9);
    case Color::Peru:
      return HEXCODE(0xcd853f);
    case Color::Pink:
      return HEXCODE(0xffc0cb);
    case Color::Plum:
      return HEXCODE(0xdda0dd);
    case Color::PowderBlue:
      return HEXCODE(0xb0e0e6);
    case Color::Rosybrown:
      return HEXCODE(0xbc8f8f);
    case Color::Royalblue:
      return HEXCODE(0x4169e1);
    case Color::SaddleBrown:
      return HEXCODE(0x8b4513);
    case Color::Salmon:
      return HEXCODE(0xfa8072);
    case Color::SandyBrown:
      return HEXCODE(0xf4a460);
    case Color::Seagreen:
      return HEXCODE(0x2e8b57);
    case Color::Seashell:
      return HEXCODE(0xfff5ee);
    case Color::Sienna:
      return HEXCODE(0xa0522d);
    case Color::SkyBlue:
      return HEXCODE(0x87ceeb);
    case Color::SlateBlue:
      return HEXCODE(0x6a5acd);
    case Color::SlateGray:
      return HEXCODE(0x708090);
    case Color::Snow:
      return HEXCODE(0xfffafa);
    case Color::SpringGreen:
      return HEXCODE(0x00ff7f);
    case Color::SteelBlue:
      return HEXCODE(0x4682b4);
    case Color::Tan:
      return HEXCODE(0xd2b48c);
    case Color::Thistle:
      return HEXCODE(0xd8bfd8);
    case Color::Tomato:
      return HEXCODE(0xff6347);
    case Color::Turquoise:
      return HEXCODE(0x40e0d0);
    case Color::Violet:
      return HEXCODE(0xee82ee);
    case Color::Wheat:
      return HEXCODE(0xf5deb3);
    case Color::WhiteSmoke:
      return HEXCODE(0xf5f5f5);
    case Color::YellowGreen:
      return HEXCODE(0x9acd32);
    default:
      std::cerr << "invalid color name\n";
      return HEXCODE(0x9acd32);
  }
}

unsigned int
ToColorHex(DawnbringerPalette color)
{
  switch(color)
  {
    case DawnbringerPalette::Blackcurrant:
      return HEXCODE(0x140C1C);
    case DawnbringerPalette::Castro:
      return HEXCODE(0x442434);
    case DawnbringerPalette::ToreaBay:
      return HEXCODE(0x30346D);
    case DawnbringerPalette::Liver:
      return HEXCODE(0x4E4A4E);
    case DawnbringerPalette::Korma:
      return HEXCODE(0x854C30);
    case DawnbringerPalette::SanFelix:
      return HEXCODE(0x346524);
    case DawnbringerPalette::Valencia:
      return HEXCODE(0xD04648);
    case DawnbringerPalette::Flint:
      return HEXCODE(0x757161);
    case DawnbringerPalette::HavelockBlue:
      return HEXCODE(0x597DCE);
    case DawnbringerPalette::Tango:
      return HEXCODE(0xD27D2C);
    case DawnbringerPalette::BaliHai:
      return HEXCODE(0x8595A1);
    case DawnbringerPalette::Sushi:
      return HEXCODE(0x6DAA2C);
    case DawnbringerPalette::Cashmere:
      return HEXCODE(0xD2AA99);
    case DawnbringerPalette::Seagull:
      return HEXCODE(0x6DC2CA);
    case DawnbringerPalette::Goldenrod:
      return HEXCODE(0xDAD45E);
    case DawnbringerPalette::Tara:
      return HEXCODE(0xDEEED6);
    default:
      std::cerr << "invalid dawnbringer palette name\n";
      return HEXCODE(0x9acd32);
  }
}
