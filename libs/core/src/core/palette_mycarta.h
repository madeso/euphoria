#pragma once

namespace euphoria::core
{
    struct palette;
}

namespace euphoria::core::palettes
{

    /*
    I created three color palettes for structure maps (seismic horizons, elevation maps, etcetera) and seismic attributes. 

    https://mycarta.wordpress.com/color-palettes/
    */

    const palette&
    cube_1();

    const palette&
    cube_yf();

    const palette&
    linear_l();
}
