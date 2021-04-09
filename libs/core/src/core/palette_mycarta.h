#ifndef EUPHORIA_PALETTE_MYCARTA_H
#define EUPHORIA_PALETTE_MYCARTA_H

namespace euphoria::core
{
    struct Palette;

    /*
I created three color palettes for structure maps (seismic horizons, elevation maps, etcetera) and seismic attributes. 

https://mycarta.wordpress.com/color-palettes/
*/

    namespace palette
    {
        const Palette&
        Cube1();

        const Palette&
        CubeYF();

        const Palette&
        LinearL();
    }  // namespace palette

}  // namespace euphoria::core

#endif  // EUPHORIA_PALETTE_MYCARTA_H
