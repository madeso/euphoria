#include "core/palette_tableu.h"

#include "core/palette.h"

namespace euphoria::core
{
    namespace
    {
        rgbi
        C(unsigned char r, unsigned char g, unsigned char b)
        {
            return {r, g, b};
        }
    }

    /*
    src: https://github.com/jiffyclub/palettable/blob/master/palettable/tableau/tableau.py

    Color palettes derived from Tableau: http://www.tableausoftware.com/
    See also:
    http://kb.tableausoftware.com/articles/knowledgebase/creating-custom-color-palettes
    http://tableaufriction.blogspot.ro/2012/11/finally-you-can-use-tableau-data-colors.html
    */
    namespace palettes
    {
        const palette&
        tableau_10()
        {
            // Tableau 10
            static const auto p = palette
            {
                "Tableau_10",
                std::array
                {
                    C(31, 119, 180),
                    C(255, 127, 14),
                    C(44, 160, 44),
                    C(214, 39, 40),
                    C(148, 103, 189),
                    C(140, 86, 75),
                    C(227, 119, 194),
                    C(127, 127, 127),
                    C(188, 189, 34),
                    C(23, 190, 207)
                }
            };
            return p;
        }


        const palette&
        tableau_light_10()
        {
            // Tableau 10 Light
            static const auto p = palette
            {
                "TableauLight_10",
                std::array
                {
                    C(174, 199, 232),
                    C(255, 187, 120),
                    C(152, 223, 138),
                    C(255, 152, 150),
                    C(197, 176, 213),
                    C(196, 156, 148),
                    C(247, 182, 210),
                    C(199, 199, 199),
                    C(219, 219, 141),
                    C(158, 218, 229)
                }
            };
            return p;
        }


        const palette&
        tableau_medium_10()
        {
            // Tableau 10 Medium
            static const auto p = palette
            {
                "TableauMedium_10",
                std::array
                {
                    C(114, 158, 206),
                    C(255, 158, 74),
                    C(103, 191, 92),
                    C(237, 102, 93),
                    C(173, 139, 201),
                    C(168, 120, 110),
                    C(237, 151, 202),
                    C(162, 162, 162),
                    C(205, 204, 93),
                    C(109, 204, 218)
                }
            };
            return p;
        }


        const palette&
        tableau_20()
        {
            // Tableau 20
            static const auto p = palette
            {
                "Tableau_20",
                std::array
                {
                    C(31, 119, 180), C(174, 199, 232),
                    C(255, 127, 14), C(255, 187, 120), C(44, 160, 44),
                    C(152, 223, 138), C(214, 39, 40), C(255, 152, 150),
                    C(148, 103, 189), C(197, 176, 213), C(140, 86, 75),
                    C(196, 156, 148), C(227, 119, 194), C(247, 182, 210),
                    C(127, 127, 127), C(199, 199, 199), C(188, 189, 34),
                    C(219, 219, 141), C(23, 190, 207), C(158, 218, 229)
                }
            };
            return p;
        }


        const palette&
        gray_5()
        {
            // Gray 5
            static const auto p = palette
            {
                "Gray_5",
                std::array
                {
                    C(96, 99, 106),
                    C(165, 172, 175),
                    C(65, 68, 81),
                    C(143, 135, 130),
                    C(207, 207, 207)
                }
            };
            return p;
        }


        const palette&
        color_blind_10()
        {
            // Color Blind 10
            static const auto p = palette
            {
                "ColorBlind_10",
                std::array
                {
                    C(0, 107, 164),
                    C(255, 128, 14),
                    C(171, 171, 171),
                    C(89, 89, 89),
                    C(95, 158, 209),
                    C(200, 82, 0),
                    C(137, 137, 137),
                    C(162, 200, 236),
                    C(255, 188, 121),
                    C(207, 207, 207)
                }
            };
            return p;
        }


        const palette&
        traffic_light_9()
        {
            // Traffic Light 9
            static const auto p = palette
            {
                "TrafficLight_9",
                std::array
                {
                    C(177, 3, 24),
                    C(219, 161, 58),
                    C(48, 147, 67),
                    C(216, 37, 38),
                    C(255, 193, 86),
                    C(105, 183, 100),
                    C(242, 108, 100),
                    C(255, 221, 113),
                    C(159, 205, 153)
                }
            };
            return p;
        }


        const palette&
        purple_gray_6()
        {
            // Purple-Gray 6
            static const auto p = palette
            {
                "PurpleGray_6",
                std::array
                {
                    C(123, 102, 210),
                    C(220, 95, 189),
                    C(148, 145, 123),
                    C(153, 86, 136),
                    C(208, 152, 238),
                    C(215, 213, 197)
                }
            };
            return p;
        }


        const palette&
        purple_gray_12()
        {
            // Purple-Gray 12
            static const auto p = palette
            {
                "PurpleGray_12",
                std::array
                {
                    C(123, 102, 210),
                    C(166, 153, 232),
                    C(220, 95, 189),
                    C(255, 192, 218),
                    C(95, 90, 65),
                    C(180, 177, 155),
                    C(153, 86, 136),
                    C(216, 152, 186),
                    C(171, 106, 213),
                    C(208, 152, 238),
                    C(139, 124, 110),
                    C(219, 212, 197)
                }
            };
            return p;
        }


        const palette&
        blue_red_6()
        {
            // Blue-Red 6
            static const auto p = palette
            {
                "BlueRed_6",
                std::array
                {
                    C(44, 105, 176),
                    C(240, 39, 32),
                    C(172, 97, 60),
                    C(107, 163, 214),
                    C(234, 107, 115),
                    C(233, 195, 155)
                }
            };
            return p;
        }


        const palette&
        blue_red_12()
        {
            // Blue-Red 12
            static const auto p = palette
            {
                "BlueRed_12",
                std::array
                {
                    C(44, 105, 176),
                    C(181, 200, 226),
                    C(240, 39, 32),
                    C(255, 182, 176),
                    C(172, 97, 60),
                    C(233, 195, 155),
                    C(107, 163, 214),
                    C(181, 223, 253),
                    C(172, 135, 99),
                    C(221, 201, 180),
                    C(189, 10, 54),
                    C(244, 115, 122)
                }
            };
            return p;
        }


        const palette&
        green_orange_6()
        {
            // Green-Orange 6
            static const auto p = palette
            {
                "GreenOrange_6",
                std::array
                {
                    C(50, 162, 81),
                    C(255, 127, 15),
                    C(60, 183, 204),
                    C(255, 217, 74),
                    C(57, 115, 124),
                    C(184, 90, 13)
                }
            };
            return p;
        }


        const palette&
        green_orange_12()
        {
            // Green-Orange 12
            static const auto p = palette
            {
                "GreenOrange_12",
                std::array
                {
                    C(50, 162, 81),
                    C(172, 217, 141),
                    C(255, 127, 15),
                    C(255, 185, 119),
                    C(60, 183, 204),
                    C(152, 217, 228),
                    C(184, 90, 13),
                    C(255, 217, 74),
                    C(57, 115, 124),
                    C(134, 180, 169),
                    C(130, 133, 59),
                    C(204, 201, 77)
                }
            };
            return p;
        }
    }

}
