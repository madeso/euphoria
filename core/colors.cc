#include "core/colors.h"

#include "core/log.h"

LOG_SPECIFY_DEFAULT_LOGGER("core.colors")

#define RGB(r, g, b) ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);

namespace euphoria::core
{
    unsigned int
    ToColorHex(Color color)
    {
        switch (color)
        {
        case Color::White: return RGB(255, 255, 255);
        case Color::LightGray: return RGB(160, 160, 160);
        case Color::Gray: return RGB(127, 127, 127);
        case Color::DarkGray: return RGB(87, 87, 87);
        case Color::Black: return RGB(0, 0, 0);
        case Color::Red: return RGB(173, 35, 35);
        case Color::PureRed: return RGB(255, 0, 0);
        case Color::Blue: return RGB(42, 75, 215);
        case Color::PureBlue: return RGB(0, 0, 255);
        case Color::LightBlue: return RGB(157, 175, 255);
        case Color::NormalBlue: return RGB(127, 127, 255);
        case Color::CornflowerBlue: return RGB(100, 149, 237);
        case Color::Green: return RGB(29, 105, 20);
        case Color::PureGreen: return RGB(0, 255, 0);
        case Color::LightGreen: return RGB(129, 197, 122);
        case Color::Yellow: return RGB(255, 238, 51);
        case Color::PureYellow: return RGB(255, 255, 0);
        case Color::Orange: return RGB(255, 146, 51);
        case Color::PureOrange: return RGB(255, 127, 0);
        case Color::Brown: return RGB(129, 74, 25);
        case Color::PureBrown: return RGB(250, 75, 0);
        case Color::Purple: return RGB(129, 38, 192);
        case Color::PurePurple: return RGB(128, 0, 128);
        case Color::Pink: return RGB(255, 205, 243);
        case Color::PurePink: return RGB(255, 192, 203);
        case Color::PureBeige: return RGB(245, 245, 220);
        case Color::Tan: return RGB(233, 222, 187);
        case Color::PureTan: return RGB(210, 180, 140);
        case Color::Cyan: return RGB(41, 208, 208);
        case Color::PureCyan: return RGB(0, 255, 255);

        default: LOG_ERROR("Invalid color name"); return RGB(0, 0, 0);
        }
    }

    BEGIN_ENUM_LIST(Color)
    ENUM_VALUE(Color, White)
    ENUM_VALUE(Color, LightGray)
    ENUM_VALUE(Color, Gray)
    ENUM_VALUE(Color, DarkGray)
    ENUM_VALUE(Color, Black)
    ENUM_VALUE(Color, Red)
    ENUM_VALUE(Color, PureRed)
    ENUM_VALUE(Color, Blue)
    ENUM_VALUE(Color, PureBlue)
    ENUM_VALUE(Color, LightBlue)
    ENUM_VALUE(Color, NormalBlue)
    ENUM_VALUE(Color, CornflowerBlue)
    ENUM_VALUE(Color, Green)
    ENUM_VALUE(Color, PureGreen)
    ENUM_VALUE(Color, LightGreen)
    ENUM_VALUE(Color, Yellow)
    ENUM_VALUE(Color, PureYellow)
    ENUM_VALUE(Color, Orange)
    ENUM_VALUE(Color, PureOrange)
    ENUM_VALUE(Color, Brown)
    ENUM_VALUE(Color, PureBrown)
    ENUM_VALUE(Color, Purple)
    ENUM_VALUE(Color, PurePurple)
    ENUM_VALUE(Color, Pink)
    ENUM_VALUE(Color, PurePink)
    ENUM_VALUE(Color, Beige)
    ENUM_VALUE(Color, PureBeige)
    ENUM_VALUE(Color, Tan)
    ENUM_VALUE(Color, PureTan)
    ENUM_VALUE(Color, Cyan)
    ENUM_VALUE(Color, PureCyan)
    END_ENUM_LIST()
}  // namespace euphoria::core
