#ifndef EUPHORIA_PALETTE_MATPLOT_H
#define EUPHORIA_PALETTE_MATPLOT_H

namespace euphoria::core
{
    struct palette;

    // https://bids.github.io/colormap/

    namespace palettes
    {
        const palette&
        Magma();
        const palette&
        Inferno();
        const palette&
        Plasma();
        const palette&
        Viridis();
    }  // namespace palette

}  // namespace euphoria::core

#endif  // EUPHORIA_PALETTE_MATPLOT_H
