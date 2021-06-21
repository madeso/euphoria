#pragma once

#include <string>
#include <array>

#include "core/enumtostring.h"

namespace euphoria::core
{
    struct palette;
}

namespace euphoria::core::palettes
{
    enum class name
    {
        // standard
        dawnbringer,
        named_colors,

        // cubehelix
        classic,
        perceptual_rainbow,
        purple,
        jim_special,
        red,
        cubehelix_1,
        cubehelix_2,
        cubehelix_3,

        // lospec
        one_bit,
        two_bit_grayscale,
        three_bit,
        arq_4,
        cga,
        endesga_4,
        endesga_8,
        endesga_16,
        endesga_32,
        endesga_36,
        endesga_64,
        ink,
        pico8,
        ammo_8,
        nyx_8,
        fifteen_p_dx,
        twenty_p_dx,
        arne_16,
        night_16,
        aap_16,
        aap_64,
        splendor_128,
        famicube,

        // matplot
        magma,
        inferno,
        plasma,
        viridis,

        // mycarta
        cube1,
        cube_yf,
        linear_l,

        // tableu
        tableau_10,
        tableau_light_10,
        tableau_medium_10,
        tableau_20,
        gray_5,
        color_blind_10,
        traffic_light_9,
        purple_gray_6,
        purple_gray_12,
        blue_red_6,
        blue_red_12,
        green_orange_6,
        green_orange_12
    };

    palette
    get_palette(name palette_name);

    constexpr std::array palette_names
    {
        // standard
        name::dawnbringer,
        name::named_colors,

        // cubehelix
        name::classic,
        name::perceptual_rainbow,
        name::purple,
        name::jim_special,
        name::red,
        name::cubehelix_1,
        name::cubehelix_2,
        name::cubehelix_3,

        // lospec
        name::one_bit,
        name::two_bit_grayscale,
        name::three_bit,
        name::arq_4,
        name::cga,
        name::endesga_8,
        name::endesga_16,
        name::endesga_32,
        name::endesga_36,
        name::endesga_64,
        name::endesga_4,
        name::ink,
        name::pico8,
        name::ammo_8,
        name::nyx_8,
        name::fifteen_p_dx,
        name::twenty_p_dx,
        name::arne_16,
        name::night_16,
        name::aap_16,
        name::aap_64,
        name::splendor_128,
        name::famicube,

        // matplot
        name::magma,
        name::inferno,
        name::plasma,
        name::viridis,

        // mycarta
        name::cube1,
        name::cube_yf,
        name::linear_l,

        // tableu
        name::tableau_10,
        name::tableau_light_10,
        name::tableau_medium_10,
        name::tableau_20,
        name::gray_5,
        name::color_blind_10,
        name::traffic_light_9,
        name::purple_gray_6,
        name::purple_gray_12,
        name::blue_red_6,
        name::blue_red_12,
        name::green_orange_6,
        name::green_orange_12
    };
}
