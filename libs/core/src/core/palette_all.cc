#include "core/palette_all.h"

#include "core/palette.h"
#include "core/palette_cubehelix.h"
#include "core/palette_lospec.h"
#include "core/palette_matplot.h"
#include "core/palette_mycarta.h"
#include "core/palette_tableu.h"


namespace euphoria::core::palettes
{
    const palette&
    get_palette(name palette_name)
    {
        switch(palette_name)
        {
        // standard
        case name::dawnbringer: return dawnbringer();
        case name::named_colors: return named_colors();

        // cubehelix
        case name::classic: return classic();
        case name::perceptual_rainbow: return perceptual_rainbow();
        case name::purple: return purple();
        case name::jim_special: return jim_special();
        case name::red: return red();
        case name::cubehelix_1: return cubehelix_1();
        case name::cubehelix_2: return cubehelix_2();
        case name::cubehelix_3: return cubehelix_3();

        // lospec
        case name::one_bit: return one_bit();
        case name::two_bit_grayscale: return two_bit_grayscale();
        case name::three_bit: return three_bit();
        case name::arq_4: return arq4();
        case name::cga: return cga();
        case name::endesga_4: return endesga_4();
        case name::endesga_8: return endesga_8();
        case name::endesga_16: return endesga_16();
        case name::endesga_32: return endesga_32();
        case name::endesga_36: return endesga_36();
        case name::endesga_64: return endesga_64();
        case name::ink: return ink();
        case name::pico8: return pico8();
        case name::ammo_8: return ammo8();
        case name::nyx_8: return nyx8();
        case name::fifteen_p_dx: return fifteen_p_dx();
        case name::twenty_p_dx: return twenty_p_dx();
        case name::arne_16: return arne16();
        case name::night_16: return night16();
        case name::aap_16: return aap16();
        case name::aap_64: return aap64();
        case name::splendor_128: return splendor128();
        case name::famicube: return famicube();

        // matplot
        case name::magma: return magma();
        case name::inferno: return inferno();
        case name::plasma: return plasma();
        case name::viridis: return viridis();

        // mycarta
        case name::cube1: return cube_1();
        case name::cube_yf: return cube_yf();
        case name::linear_l: return linear_l();

        // tableu
        case name::tableau_10: return tableau_10();
        case name::tableau_light_10: return TableauLight_10();
        case name::tableau_medium_10: return tableau_medium_10();
        case name::tableau_20: return tableau_20();
        case name::gray_5: return gray_5();
        case name::color_blind_10: return color_blind_10();
        case name::traffic_light_9: return traffic_light_9();
        case name::purple_gray_6: return purple_gray_6();
        case name::purple_gray_12: return purple_gray_12();
        case name::blue_red_6: return blue_red_6();
        case name::blue_red_12: return blue_red_12();
        case name::green_orange_6: return green_orange_6();
        case name::green_orange_12: return green_orange_12();

        default: return one_bit();
        }
    }
}
