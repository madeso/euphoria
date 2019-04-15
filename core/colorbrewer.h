#ifndef CORE_COLORBREWER_H
#define CORE_COLORBREWER_H

#include "core/palette.h"

#include <map>
#include <vector>

namespace colorbrewer
{
  enum class Type
  {
    div, qual, seq
  };

  enum class Class
  {
    Not   = 0,
    Ok    = 1,
    Maybe = 2 // or possible not
  };

  using Blind = std::vector<int>;
  using Print = std::vector<int>;
  using Copy  = std::vector<int>;
  using Screen = std::vector<int>;

  struct Properties
  {
    Type type;

    Blind blind;
    Print print;
    Copy copy;
    Screen screen;
  };

  using Map = std::map<int, Palette>;

  struct ColorBrewer
  {
    Map palettes;
    Properties             properties;
  };
}  // namespace colorbrewer

namespace colorbrewer
{
/*** Diverging ***/
  const ColorBrewer& Spectral();
  const ColorBrewer& RdYlGn();
  const ColorBrewer& RdBu();
  const ColorBrewer& PiYG();
  const ColorBrewer& PRGn();
  const ColorBrewer& RdYlBu();
  const ColorBrewer& BrBG();
  const ColorBrewer& RdGy();
  const ColorBrewer& PuOr();

  /*** Qualitative ***/
  const ColorBrewer& Set2();
  const ColorBrewer& Accent();
  const ColorBrewer& Set1();
  const ColorBrewer& Set3();
  const ColorBrewer& Dark2();
  const ColorBrewer& Paired();
  const ColorBrewer& Pastel2();
  const ColorBrewer& Pastel1();

  /*** Sequential ***/
  const ColorBrewer& OrRd();
  const ColorBrewer& PuBu();
  const ColorBrewer& BuPu();
  const ColorBrewer& Oranges();
  const ColorBrewer& BuGn();
  const ColorBrewer& YlOrBr();
  const ColorBrewer& YlGn();
  const ColorBrewer& Reds();
  const ColorBrewer& RdPu();
  const ColorBrewer& Greens();
  const ColorBrewer& YlGnBu();
  const ColorBrewer& Purples();
  const ColorBrewer& GnBu();
  const ColorBrewer& Greys();
  const ColorBrewer& YlOrRd();
  const ColorBrewer& PuRd();
  const ColorBrewer& Blues();
  const ColorBrewer& PuBuGn();

}  // namespace colorbrewer

#endif CORE_COLORBREWER_H
