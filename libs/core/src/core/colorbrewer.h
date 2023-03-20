#pragma once

#include "core/palette.h"

#include <map>


namespace euphoria::core
{
    namespace colorbrewer
    {
        enum class BrewerType
        {
            div,
            qual,
            seq
        };

        enum class BrewerClass
        {
            no = 0,
            yes = 1,
            maybe = 2
        };

        using BlindProperties = std::vector<int>;
        using PrintProperties = std::vector<int>;
        using CopyProperties = std::vector<int>;
        using ScreenProperties = std::vector<int>;

        struct Properties
        {
            BlindProperties blind;
            PrintProperties print;
            CopyProperties copy;
            ScreenProperties screen;
        };

        struct BrewerPalette
        {
            // todo(Gustav): move Properies here
            core::Palette palette;
        };

        using SizeToPaletteMap = std::map<int, BrewerPalette>;

        struct Colorbrewer
        {
            std::string name;
            SizeToPaletteMap palettes;
            BrewerType type;
            Properties props;
        };

        struct SinglePalette
        {
            std::string name;
            BrewerPalette palette;
        };
    }

    namespace colorbrewer
    {
        const std::vector<Colorbrewer>&
        get_all_palettes();

        std::vector<SinglePalette>
        find_palette(BrewerType* type, int* size);

        /*** Diverging ***/
        const Colorbrewer& get_palette_diverging_spectral();
        const Colorbrewer& get_palette_diverging_rd_yl_gn();
        const Colorbrewer& get_palette_diverging_rd_bu();
        const Colorbrewer& get_palette_diverging_pi_y_g();
        const Colorbrewer& get_palette_diverging_p_r_gn();
        const Colorbrewer& get_palette_diverging_rd_yl_bu();
        const Colorbrewer& get_palette_diverging_br_b_g();
        const Colorbrewer& get_palette_diverging_rd_gy();
        const Colorbrewer& get_palette_diverging_pu_or();

        /*** Qualitative ***/
        const Colorbrewer& get_palette_qualitative_set2();
        const Colorbrewer& get_palette_qualitative_accent();
        const Colorbrewer& get_palette_qualitative_set1();
        const Colorbrewer& get_palette_qualitative_set3();
        const Colorbrewer& get_palette_qualitative_dark2();
        const Colorbrewer& get_palette_qualitative_paired();
        const Colorbrewer& get_palette_qualitative_pastel2();
        const Colorbrewer& get_palette_qualitative_pastel1();

        /*** Sequential ***/
        const Colorbrewer& get_palette_sequential_or_rd();
        const Colorbrewer& get_palette_sequential_pu_bu();
        const Colorbrewer& get_palette_sequential_bu_pu();
        const Colorbrewer& get_palette_sequential_oranges();
        const Colorbrewer& get_palette_sequential_bu_gn();
        const Colorbrewer& get_palette_sequential_yl_or_br();
        const Colorbrewer& get_palette_sequential_yl_gn();
        const Colorbrewer& get_palette_sequential_reds();
        const Colorbrewer& get_palette_sequential_rd_pu();
        const Colorbrewer& get_palette_sequential_greens();
        const Colorbrewer& get_palette_sequential_yl_gn_bu();
        const Colorbrewer& get_palette_sequential_purples();
        const Colorbrewer& get_palette_sequential_gn_bu();
        const Colorbrewer& get_palette_sequential_greys();
        const Colorbrewer& get_palette_sequential_yl_or_rd();
        const Colorbrewer& get_palette_sequential_pu_rd();
        const Colorbrewer& get_palette_sequential_blues();
        const Colorbrewer& get_palette_sequential_pu_bu_gn();
    }
}
