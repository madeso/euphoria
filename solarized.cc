
#include "solarized.h"

Solarized::Solarized(bool light)
    : really_strong_border(
          light ? solarized_light::base03 : solarized_light::base3)
    , strong_border(light ? solarized_light::base02 : solarized_light::base2)
    , emphasized_content(
          light ? solarized_light::base01 : solarized_light::base1)
    , primary_content(light ? solarized_light::base00 : solarized_light::base0)
    , not_used(light ? solarized_light::base0 : solarized_light::base00)
    , comments(light ? solarized_light::base1 : solarized_light::base01)
    , background_highlight(
          light ? solarized_light::base2 : solarized_light::base02)
    , background(light ? solarized_light::base3 : solarized_light::base03)
    , yellow(solarized_light::yellow)
    , orange(solarized_light::orange)
    , red(solarized_light::red)
    , magenta(solarized_light::magenta)
    , violet(solarized_light::violet)
    , blue(solarized_light::blue)
    , cyan(solarized_light::cyan)
    , green(solarized_light::green)
{
}
