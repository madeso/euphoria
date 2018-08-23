#ifndef SOLARIZED_H
#define SOLARIZED_H

using Color = int;

// solarized light color spec
namespace solarized_light
{
  constexpr Color base03  = 0x002B36FF;
  constexpr Color base02  = 0x073642FF;
  constexpr Color base01  = 0x586E75FF;
  constexpr Color base00  = 0x657B83FF;
  constexpr Color base0   = 0x839496FF;
  constexpr Color base1   = 0x93A1A1FF;
  constexpr Color base2   = 0xEEE8D5FF;
  constexpr Color base3   = 0xFDF6E3FF;
  constexpr Color yellow  = 0xB58900FF;
  constexpr Color orange  = 0xCB4B16FF;
  constexpr Color red     = 0xDC322FFF;
  constexpr Color magenta = 0xD33682FF;
  constexpr Color violet  = 0x6C71C4FF;
  constexpr Color blue    = 0x268BD2FF;
  constexpr Color cyan    = 0x2AA198FF;
  constexpr Color green   = 0x859900FF;
}  // namespace solarized_light

struct Solarized
{
  explicit Solarized(bool light);

  Color really_strong_border;
  Color strong_border;
  Color emphasized_content;
  Color primary_content;
  Color not_used;
  Color comments;
  Color background_highlight;
  Color background;

  Color yellow;
  Color orange;
  Color red;
  Color magenta;
  Color violet;
  Color blue;
  Color cyan;
  Color green;
};

#endif  // SOLARIZED_H
