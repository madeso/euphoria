#ifndef EUPHORIA_PALETTE_MATPLOT_H
#define EUPHORIA_PALETTE_MATPLOT_H

namespace euphoria::core
{

struct Palette;

// https://bids.github.io/colormap/

namespace palette
{
  const Palette& Magma();
  const Palette& Inferno();
  const Palette& Plasma();
  const Palette& Viridis();
}

}

#endif  // EUPHORIA_PALETTE_MATPLOT_H

