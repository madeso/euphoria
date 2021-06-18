#pragma once

#include "core/palette.h"

#include <map>
#include <vector>

namespace euphoria::core
{
    namespace colorbrewer
    {
        enum class brewer_type
        {
            div,
            qual,
            seq
        };

        enum class brewer_class
        {
            no = 0,
            yes = 1,
            maybe = 2
        };

        using blind_properties = std::vector<int>;
        using print_properties = std::vector<int>;
        using copy_properties = std::vector<int>;
        using screen_properties = std::vector<int>;

        struct properties
        {
            blind_properties blind;
            print_properties print;
            copy_properties copy;
            screen_properties screen;
        };

        struct brewer_palette
        {
            // todo(Gustav): move Properies here
            core::palette palette;
        };

        using size_to_palette_mapp = std::map<int, brewer_palette>;

        struct colorbrewer
        {
            std::string name;
            size_to_palette_mapp palettes;
            brewer_type type;
            properties properties;
        };

        struct single_palette
        {
            std::string name;
            brewer_palette palette;
        };
    }

    namespace colorbrewer
    {
        const std::vector<colorbrewer>&
        get_all_palettes();

        std::vector<single_palette>
        find_palette(brewer_type* type, int* size);

        /*** Diverging ***/
        const colorbrewer& palette_diverging_spectral();
        const colorbrewer& palette_diverging_rd_yl_gn();
        const colorbrewer& palette_diverging_rd_bu();
        const colorbrewer& palette_diverging_pi_y_g();
        const colorbrewer& palette_diverging_p_r_gn();
        const colorbrewer& palette_diverging_rd_yl_bu();
        const colorbrewer& palette_diverging_br_b_g();
        const colorbrewer& palette_diverging_rd_gy();
        const colorbrewer& palette_diverging_pu_or();

        /*** Qualitative ***/
        const colorbrewer& palette_qualitative_set2();
        const colorbrewer& palette_qualitative_accent();
        const colorbrewer& palette_qualitative_set1();
        const colorbrewer& palette_qualitative_set3();
        const colorbrewer& palette_qualitative_dark2();
        const colorbrewer& palette_qualitative_paired();
        const colorbrewer& palette_qualitative_pastel2();
        const colorbrewer& palette_qualitative_pastel1();

        /*** Sequential ***/
        const colorbrewer& palette_sequential_or_rd();
        const colorbrewer& palette_sequential_pu_bu();
        const colorbrewer& palette_sequential_bu_pu();
        const colorbrewer& palette_sequential_oranges();
        const colorbrewer& palette_sequential_bu_gn();
        const colorbrewer& palette_sequential_yl_or_br();
        const colorbrewer& palette_sequential_yl_gn();
        const colorbrewer& palette_sequential_reds();
        const colorbrewer& palette_sequential_rd_pu();
        const colorbrewer& palette_sequential_greens();
        const colorbrewer& palette_sequential_yl_gn_bu();
        const colorbrewer& palette_sequential_purples();
        const colorbrewer& palette_sequential_gn_bu();
        const colorbrewer& palette_sequential_greys();
        const colorbrewer& palette_sequential_yl_or_rd();
        const colorbrewer& palette_sequential_pu_rd();
        const colorbrewer& palette_sequential_blues();
        const colorbrewer& palette_sequential_pu_bu_gn();

    }
}
