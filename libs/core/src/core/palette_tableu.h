#pragma once

#include "core/palette.h"


/*
src: https://github.com/jiffyclub/palettable/blob/master/palettable/tableau/tableau.py

Color palettes derived from Tableau: http://www.tableausoftware.com/
See also:
http://kb.tableausoftware.com/articles/knowledgebase/creating-custom-color-palettes
http://tableaufriction.blogspot.ro/2012/11/finally-you-can-use-tableau-data-colors.html
*/

namespace euphoria::core::palettes::tableau
{
    constexpr Rgbi
    con(unsigned char r, unsigned char g, unsigned char b)
    {
        return {r, g, b};
    }

    constexpr auto tableau_10 = make_static_palette
    (
        // Tableau 10
        "Tableau_10",
        con(31, 119, 180),
        con(255, 127, 14),
        con(44, 160, 44),
        con(214, 39, 40),
        con(148, 103, 189),
        con(140, 86, 75),
        con(227, 119, 194),
        con(127, 127, 127),
        con(188, 189, 34),
        con(23, 190, 207)
    );


    constexpr auto tableau_light_10 = make_static_palette
    (
        // Tableau 10 Light
        "TableauLight_10",
        con(174, 199, 232),
        con(255, 187, 120),
        con(152, 223, 138),
        con(255, 152, 150),
        con(197, 176, 213),
        con(196, 156, 148),
        con(247, 182, 210),
        con(199, 199, 199),
        con(219, 219, 141),
        con(158, 218, 229)
    );


    constexpr auto tableau_medium_10 = make_static_palette
    (
        // Tableau 10 Medium
        "TableauMedium_10",
        con(114, 158, 206),
        con(255, 158, 74),
        con(103, 191, 92),
        con(237, 102, 93),
        con(173, 139, 201),
        con(168, 120, 110),
        con(237, 151, 202),
        con(162, 162, 162),
        con(205, 204, 93),
        con(109, 204, 218)
    );


    constexpr auto tableau_20 = make_static_palette
    (
        // Tableau 20
        "Tableau_20",
        con(31, 119, 180), con(174, 199, 232),
        con(255, 127, 14), con(255, 187, 120), con(44, 160, 44),
        con(152, 223, 138), con(214, 39, 40), con(255, 152, 150),
        con(148, 103, 189), con(197, 176, 213), con(140, 86, 75),
        con(196, 156, 148), con(227, 119, 194), con(247, 182, 210),
        con(127, 127, 127), con(199, 199, 199), con(188, 189, 34),
        con(219, 219, 141), con(23, 190, 207), con(158, 218, 229)
    );


    constexpr auto gray_5 = make_static_palette
    (
        // Gray 5
        "Gray_5",
        con(96, 99, 106),
        con(165, 172, 175),
        con(65, 68, 81),
        con(143, 135, 130),
        con(207, 207, 207)
    );


    constexpr auto color_blind_10 = make_static_palette
    (
        // Color Blind 10
        "ColorBlind_10",
        con(0, 107, 164),
        con(255, 128, 14),
        con(171, 171, 171),
        con(89, 89, 89),
        con(95, 158, 209),
        con(200, 82, 0),
        con(137, 137, 137),
        con(162, 200, 236),
        con(255, 188, 121),
        con(207, 207, 207)
    );


    constexpr auto traffic_light_9 = make_static_palette
    (
        // Traffic Light 9
        "TrafficLight_9",
        con(177, 3, 24),
        con(219, 161, 58),
        con(48, 147, 67),
        con(216, 37, 38),
        con(255, 193, 86),
        con(105, 183, 100),
        con(242, 108, 100),
        con(255, 221, 113),
        con(159, 205, 153)
    );


    constexpr auto purple_gray_6 = make_static_palette
    (
        // Purple-Gray 6
        "PurpleGray_6",
        con(123, 102, 210),
        con(220, 95, 189),
        con(148, 145, 123),
        con(153, 86, 136),
        con(208, 152, 238),
        con(215, 213, 197)
    );


    constexpr auto purple_gray_12 = make_static_palette
    (
        // Purple-Gray 12
        "PurpleGray_12",
        con(123, 102, 210),
        con(166, 153, 232),
        con(220, 95, 189),
        con(255, 192, 218),
        con(95, 90, 65),
        con(180, 177, 155),
        con(153, 86, 136),
        con(216, 152, 186),
        con(171, 106, 213),
        con(208, 152, 238),
        con(139, 124, 110),
        con(219, 212, 197)
    );


    constexpr auto blue_red_6 = make_static_palette
    (
        // Blue-Red 6
        "BlueRed_6",
        con(44, 105, 176),
        con(240, 39, 32),
        con(172, 97, 60),
        con(107, 163, 214),
        con(234, 107, 115),
        con(233, 195, 155)
    );


    constexpr auto blue_red_12 = make_static_palette
    (
        // Blue-Red 12
        "BlueRed_12",
        con(44, 105, 176),
        con(181, 200, 226),
        con(240, 39, 32),
        con(255, 182, 176),
        con(172, 97, 60),
        con(233, 195, 155),
        con(107, 163, 214),
        con(181, 223, 253),
        con(172, 135, 99),
        con(221, 201, 180),
        con(189, 10, 54),
        con(244, 115, 122)
    );


    constexpr auto green_orange_6 = make_static_palette
    (
        // Green-Orange 6
        "GreenOrange_6",
        con(50, 162, 81),
        con(255, 127, 15),
        con(60, 183, 204),
        con(255, 217, 74),
        con(57, 115, 124),
        con(184, 90, 13)
    );


    constexpr auto green_orange_12 = make_static_palette
    (
        // Green-Orange 12
        "GreenOrange_12",
        con(50, 162, 81),
        con(172, 217, 141),
        con(255, 127, 15),
        con(255, 185, 119),
        con(60, 183, 204),
        con(152, 217, 228),
        con(184, 90, 13),
        con(255, 217, 74),
        con(57, 115, 124),
        con(134, 180, 169),
        con(130, 133, 59),
        con(204, 201, 77)
    );
}
