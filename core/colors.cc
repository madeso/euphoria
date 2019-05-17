#include "core/colors.h"

#include "core/log.h"

LOG_SPECIFY_DEFAULT_LOGGER("core.colors")

#define RGB(r, g, b)  ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);

unsigned int
ToColorHex(Color color)
{
  switch(color)
  {
    case Color::Black:      return RGB( 0, 0, 0);
    case Color::DarkGray:   return RGB( 87, 87, 87);
    case Color::Gray:       return RGB( 123, 123, 123);
    case Color::LightGray:  return RGB( 160, 160, 160);
    case Color::Red:        return RGB( 173, 35, 35);
    case Color::Blue:       return RGB( 42, 75, 215);
    case Color::Green:      return RGB( 29, 105, 20);
    case Color::Brown:      return RGB( 129, 74, 25);
    case Color::Purple:     return RGB( 129, 38, 192);
    case Color::LightGreen: return RGB( 129, 197, 122);
    case Color::LightBlue:  return RGB( 157, 175, 255);
    case Color::Cyan:       return RGB( 41, 208, 208);
    case Color::Orange:     return RGB( 255, 146, 51);
    case Color::Yellow:     return RGB( 255, 238, 51);
    case Color::Tan:        return RGB( 233, 222, 187);
    case Color::Pink:       return RGB( 255, 205, 243);
    case Color::White:      return RGB( 255, 255, 255);
    case Color::Beige:      return RGB( 245, 245, 220);

    default:
      LOG_ERROR("Invalid color name");
      return RGB(0,0,0);
  }
}

