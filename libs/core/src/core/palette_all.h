#pragma once


#include <array>

#include "core/enumtostring.h"
#include "core/compiler_warning.h"

namespace euphoria::core
{
    struct Palette;
}

namespace euphoria::core::palettes
{
    enum class Name
    {
        // standard
        dawnbringer,
        get_named_colors,

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

    Palette
    get_palette(Name palette_name);

    DISABLE_WARNING_PUSH
    DISABLE_WARNING_UNUSED_VARIABLE
    // clang claims this variable is unused, but it's part of a interface

    constexpr std::array palette_names
    {
        // standard
        Name::dawnbringer,
        Name::get_named_colors,

        // cubehelix
        Name::classic,
        Name::perceptual_rainbow,
        Name::purple,
        Name::jim_special,
        Name::red,
        Name::cubehelix_1,
        Name::cubehelix_2,
        Name::cubehelix_3,

        // lospec
        Name::one_bit,
        Name::two_bit_grayscale,
        Name::three_bit,
        Name::arq_4,
        Name::cga,
        Name::endesga_8,
        Name::endesga_16,
        Name::endesga_32,
        Name::endesga_36,
        Name::endesga_64,
        Name::endesga_4,
        Name::ink,
        Name::pico8,
        Name::ammo_8,
        Name::nyx_8,
        Name::fifteen_p_dx,
        Name::twenty_p_dx,
        Name::arne_16,
        Name::night_16,
        Name::aap_16,
        Name::aap_64,
        Name::splendor_128,
        Name::famicube,

        // matplot
        Name::magma,
        Name::inferno,
        Name::plasma,
        Name::viridis,

        // mycarta
        Name::cube1,
        Name::cube_yf,
        Name::linear_l,

        // tableu
        Name::tableau_10,
        Name::tableau_light_10,
        Name::tableau_medium_10,
        Name::tableau_20,
        Name::gray_5,
        Name::color_blind_10,
        Name::traffic_light_9,
        Name::purple_gray_6,
        Name::purple_gray_12,
        Name::blue_red_6,
        Name::blue_red_12,
        Name::green_orange_6,
        Name::green_orange_12
    };
    DISABLE_WARNING_POP
}
