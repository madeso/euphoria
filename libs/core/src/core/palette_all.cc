#include "core/palette_all.h"

#include "core/palette.h"
#include "core/palette_cubehelix.h"
#include "core/palette_lospec.h"
#include "core/palette_matplot.h"
#include "core/palette_mycarta.h"
#include "core/palette_tableu.h"


namespace euphoria::core::palettes
{
    palette
    get_palette(name palette_name)
    {
        switch(palette_name)
        {
        // standard
        case name::dawnbringer: return dawnbringer();
        case name::named_colors: return named_colors();

        // cubehelix
        case name::classic: return *cubehelix::classic;
        case name::perceptual_rainbow: return *cubehelix::perceptual_rainbow;
        case name::purple: return *cubehelix::purple;
        case name::jim_special: return *cubehelix::jim_special;
        case name::red: return *cubehelix::red;
        case name::cubehelix_1: return *cubehelix::cubehelix_1;
        case name::cubehelix_2: return *cubehelix::cubehelix_2;
        case name::cubehelix_3: return *cubehelix::cubehelix_3;

        // lospec
        case name::one_bit: return *lospec::one_bit;
        case name::two_bit_grayscale: return *lospec::two_bit_grayscale;
        case name::three_bit: return *lospec::three_bit;
        case name::arq_4: return *lospec::arq4;
        case name::cga: return *lospec::cga;
        case name::endesga_4: return *lospec::endesga_4;
        case name::endesga_8: return *lospec::endesga_8;
        case name::endesga_16: return *lospec::endesga_16;
        case name::endesga_32: return *lospec::endesga_32;
        case name::endesga_36: return *lospec::endesga_36;
        case name::endesga_64: return *lospec::endesga_64;
        case name::ink: return *lospec::ink;
        case name::pico8: return *lospec::pico8;
        case name::ammo_8: return *lospec::ammo8;
        case name::nyx_8: return *lospec::nyx8;
        case name::fifteen_p_dx: return *lospec::fifteen_p_dx;
        case name::twenty_p_dx: return *lospec::twenty_p_dx;
        case name::arne_16: return *lospec::arne16;
        case name::night_16: return *lospec::night16;
        case name::aap_16: return *lospec::aap16;
        case name::aap_64: return *lospec::aap64;
        case name::splendor_128: return *lospec::splendor128;
        case name::famicube: return *lospec::famicube;

        // matplot
        case name::magma: return *matplot::magma;
        case name::inferno: return *matplot::inferno;
        case name::plasma: return *matplot::plasma;
        case name::viridis: return *matplot::viridis;

        // mycarta
        case name::cube1: return *mycarta::cube_1;
        case name::cube_yf: return *mycarta::cube_yf;
        case name::linear_l: return *mycarta::linear_l;

        // tableu
        case name::tableau_10: return *tableau::tableau_10;
        case name::tableau_light_10: return *tableau::tableau_light_10;
        case name::tableau_medium_10: return *tableau::tableau_medium_10;
        case name::tableau_20: return *tableau::tableau_20;
        case name::gray_5: return *tableau::gray_5;
        case name::color_blind_10: return *tableau::color_blind_10;
        case name::traffic_light_9: return *tableau::traffic_light_9;
        case name::purple_gray_6: return *tableau::purple_gray_6;
        case name::purple_gray_12: return *tableau::purple_gray_12;
        case name::blue_red_6: return *tableau::blue_red_6;
        case name::blue_red_12: return *tableau::blue_red_12;
        case name::green_orange_6: return *tableau::green_orange_6;
        case name::green_orange_12: return *tableau::green_orange_12;

        default: return *lospec::one_bit;
        }
    }
}
