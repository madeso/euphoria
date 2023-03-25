#include "core/palette_all.h"

#include "core/palette.h"
#include "core/palette_cubehelix.h"
#include "core/palette_lospec.h"
#include "core/palette_matplot.h"
#include "core/palette_mycarta.h"
#include "core/palette_tableu.h"


namespace euphoria::core::palettes
{
    Palette
    get_palette(Name palette_name)
    {
        switch(palette_name)
        {
        // standard
        case Name::dawnbringer: return *dawnbringer_palette;
        case Name::get_named_colors: return *named_colors_palette;

        // cubehelix
        case Name::classic: return *cubehelix::classic;
        case Name::perceptual_rainbow: return *cubehelix::perceptual_rainbow;
        case Name::purple: return *cubehelix::purple;
        case Name::jim_special: return *cubehelix::jim_special;
        case Name::red: return *cubehelix::red;
        case Name::cubehelix_1: return *cubehelix::cubehelix_1;
        case Name::cubehelix_2: return *cubehelix::cubehelix_2;
        case Name::cubehelix_3: return *cubehelix::cubehelix_3;

        // lospec
        case Name::one_bit: return *lospec::one_bit;
        case Name::two_bit_grayscale: return *lospec::two_bit_grayscale;
        case Name::three_bit: return *lospec::three_bit;
        case Name::arq_4: return *lospec::arq4;
        case Name::cga: return *lospec::cga;
        case Name::endesga_4: return *lospec::endesga_4;
        case Name::endesga_8: return *lospec::endesga_8;
        case Name::endesga_16: return *lospec::endesga_16;
        case Name::endesga_32: return *lospec::endesga_32;
        case Name::endesga_36: return *lospec::endesga_36;
        case Name::endesga_64: return *lospec::endesga_64;
        case Name::ink: return *lospec::ink;
        case Name::pico8: return *lospec::pico8;
        case Name::ammo_8: return *lospec::ammo8;
        case Name::nyx_8: return *lospec::nyx8;
        case Name::fifteen_p_dx: return *lospec::fifteen_p_dx;
        case Name::twenty_p_dx: return *lospec::twenty_p_dx;
        case Name::arne_16: return *lospec::arne16;
        case Name::night_16: return *lospec::night16;
        case Name::aap_16: return *lospec::aap16;
        case Name::aap_64: return *lospec::aap64;
        case Name::splendor_128: return *lospec::splendor128;
        case Name::famicube: return *lospec::famicube;

        // matplot
        case Name::magma: return *matplot::magma;
        case Name::inferno: return *matplot::inferno;
        case Name::plasma: return *matplot::plasma;
        case Name::viridis: return *matplot::viridis;

        // mycarta
        case Name::cube1: return *mycarta::cube_1;
        case Name::cube_yf: return *mycarta::cube_yf;
        case Name::linear_l: return *mycarta::linear_l;

        // tableu
        case Name::tableau_10: return *tableau::tableau_10;
        case Name::tableau_light_10: return *tableau::tableau_light_10;
        case Name::tableau_medium_10: return *tableau::tableau_medium_10;
        case Name::tableau_20: return *tableau::tableau_20;
        case Name::gray_5: return *tableau::gray_5;
        case Name::color_blind_10: return *tableau::color_blind_10;
        case Name::traffic_light_9: return *tableau::traffic_light_9;
        case Name::purple_gray_6: return *tableau::purple_gray_6;
        case Name::purple_gray_12: return *tableau::purple_gray_12;
        case Name::blue_red_6: return *tableau::blue_red_6;
        case Name::blue_red_12: return *tableau::blue_red_12;
        case Name::green_orange_6: return *tableau::green_orange_6;
        case Name::green_orange_12: return *tableau::green_orange_12;

        default: return *lospec::one_bit;
        }
    }
}
