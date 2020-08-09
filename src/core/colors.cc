#include "core/colors.h"

#include "core/log.h"

LOG_SPECIFY_DEFAULT_LOGGER("core.colors")

#define RGB(r, g, b) ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);

namespace euphoria::core
{
    unsigned int
    ToColorHex(Color color)
    {
        switch(color)
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
}  // namespace euphoria::core
