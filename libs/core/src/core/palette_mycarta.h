#ifndef EUPHORIA_PALETTE_MYCARTA_H
#define EUPHORIA_PALETTE_MYCARTA_H

namespace euphoria::core
{
    struct palette;

    /*
I created three color palettes for structure maps (seismic horizons, elevation maps, etcetera) and seismic attributes. 

https://mycarta.wordpress.com/color-palettes/
*/

    namespace palettes
    {
        const palette&
        Cube1();

        const palette&
        CubeYF();

        const palette&
        LinearL();
    }  // namespace palette

}  // namespace euphoria::core

#endif  // EUPHORIA_PALETTE_MYCARTA_H
