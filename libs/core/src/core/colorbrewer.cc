#include "core/colorbrewer.h"

#include "core/palette.h"

#include <map>

/*
 * This product includes color specifications and designs developed by Cynthia
 * Brewer (http://colorbrewer.org/).

 https://groups.google.com/forum/?fromgroups=#!topic/d3-js/iyXFgJR1JY0
 */

namespace euphoria::core
{
    namespace
    {
        Rgbi
        crgb(unsigned char r, unsigned char g, unsigned char b)
        {
            return Rgbi {r, g, b};
        }
    }

    namespace colorbrewer
    {
        const std::vector<Colorbrewer>&
        get_all_palettes()
        {
            static const std::vector<Colorbrewer> all =
            {
                /*** Diverging ***/
                palette_diverging_spectral(), palette_diverging_rd_yl_gn(), palette_diverging_rd_bu(), palette_diverging_pi_y_g(), palette_diverging_p_r_gn(), palette_diverging_rd_yl_bu(),
                palette_diverging_br_b_g(), palette_diverging_rd_gy(), palette_diverging_pu_or(),

                /*** Qualitative ***/
                palette_qualitative_set2(), palette_qualitative_accent(), palette_qualitative_set1(), palette_qualitative_set3(), palette_qualitative_dark2(), palette_qualitative_paired(),
                palette_qualitative_pastel2(), palette_qualitative_pastel1(),

                /*** Sequential ***/
                palette_sequential_or_rd(), palette_sequential_pu_bu(), palette_sequential_bu_pu(), palette_sequential_oranges(), palette_sequential_bu_gn(), palette_sequential_yl_or_br(), palette_sequential_yl_gn(),
                palette_sequential_reds(), palette_sequential_rd_pu(), palette_sequential_greens(), palette_sequential_yl_gn_bu(), palette_sequential_purples(), palette_sequential_gn_bu(), palette_sequential_greys(),
                palette_sequential_yl_or_rd(), palette_sequential_pu_rd(), palette_sequential_blues(), palette_sequential_pu_bu_gn()
            };
            return all;
        }

        std::vector<SinglePalette>
        find_palette(BrewerType* type, int* size)
        {
            // todo(Gustav): implement better search
            std::vector<SinglePalette> r;
            for(const auto& br: get_all_palettes())
            {
                if(type != nullptr)
                {
                    if(br.type != *type)
                    {
                        continue;
                    }
                }
                if(size != nullptr)
                {
                    auto found = br.palettes.find(*size);
                    if(found != br.palettes.end())
                    {
                        r.push_back(SinglePalette {br.name, found->second});
                    }
                }
                else
                {
                    for(const auto& p: br.palettes)
                    {
                        r.push_back(SinglePalette {br.name, p.second});
                    }
                }
            }
            return r;
        }

        /*** Diverging ***/
        const Colorbrewer&
        palette_diverging_spectral()
        {
            static const Colorbrewer cb = Colorbrewer
            {
                    "Spectral",
                    SizeToPaletteMap
                {
                    {
                        3,
                        BrewerPalette
                        {
                            Palette
                            {
                                "Spectral",
                                std::array
                                {
                                    crgb(252, 141, 89),
                                    crgb(255, 255, 191),
                                    crgb(153, 213, 148)
                                }
                            }
                        }
                    },
                    {
                        4,
                        BrewerPalette
                        {
                            Palette
                            {
                                "Spectral",
                                std::array
                                {
                                    crgb(215, 25, 28),
                                    crgb(253, 174, 97),
                                    crgb(171, 221, 164),
                                    crgb(43, 131, 186)
                                }
                            }
                        }
                    },
                    {
                        5,
                        BrewerPalette
                        {
                            Palette
                            {
                                "Spectral",
                                std::array
                                {
                                    crgb(215, 25, 28),
                                    crgb(253, 174, 97),
                                    crgb(255, 255, 191),
                                    crgb(171, 221, 164),
                                    crgb(43, 131, 186)
                                }
                            }
                        }
                    },
                    {
                        6,
                        BrewerPalette
                        {
                            Palette
                            {
                                "Spectral",
                                std::array
                                {
                                    crgb(213, 62, 79),
                                    crgb(252, 141, 89),
                                    crgb(254, 224, 139),
                                    crgb(230, 245, 152),
                                    crgb(153, 213, 148),
                                    crgb(50, 136, 189)
                                }
                            }
                        }
                    },
                    {
                        7,
                        BrewerPalette
                        {
                            Palette
                            {
                                "Spectral",
                                std::array
                                {
                                    crgb(213, 62, 79),
                                    crgb(252, 141, 89),
                                    crgb(254, 224, 139),
                                    crgb(255, 255, 191),
                                    crgb(230, 245, 152),
                                    crgb(153, 213, 148),
                                    crgb(50, 136, 189)
                                }
                            }
                        }
                    },
                    {
                        8,
                        BrewerPalette
                        {
                            Palette
                            {
                                "Spectral",
                                std::array
                                {
                                    crgb(213, 62, 79),
                                    crgb(244, 109, 67),
                                    crgb(253, 174, 97),
                                    crgb(254, 224, 139),
                                    crgb(230, 245, 152),
                                    crgb(171, 221, 164),
                                    crgb(102, 194, 165),
                                    crgb(50, 136, 189)
                                }
                            }
                        }
                    },
                    {
                        9,
                        BrewerPalette
                        {
                            Palette
                            {
                                "Spectral",
                                std::array
                                {
                                    crgb(213, 62, 79),
                                    crgb(244, 109, 67),
                                    crgb(253, 174, 97),
                                    crgb(254, 224, 139),
                                    crgb(255, 255, 191),
                                    crgb(230, 245, 152),
                                    crgb(171, 221, 164),
                                    crgb(102, 194, 165),
                                    crgb(50, 136, 189)
                                }
                            }
                        }
                    },
                    {
                        10,
                        BrewerPalette
                        {
                            Palette
                            {
                                "Spectral",
                                std::array
                                {
                                    crgb(158, 1, 66),
                                    crgb(213, 62, 79),
                                    crgb(244, 109, 67),
                                    crgb(253, 174, 97),
                                    crgb(254, 224, 139),
                                    crgb(230, 245, 152),
                                    crgb(171, 221, 164),
                                    crgb(102, 194, 165),
                                    crgb(50, 136, 189),
                                    crgb(94, 79, 162)
                                }
                            }
                        }
                    },
                    {
                        11,
                        BrewerPalette
                        {
                            Palette
                            {
                                "Spectral",
                                std::array
                                {
                                    crgb(158, 1, 66),
                                    crgb(213, 62, 79),
                                    crgb(244, 109, 67),
                                    crgb(253, 174, 97),
                                    crgb(254, 224, 139),
                                    crgb(255, 255, 191),
                                    crgb(230, 245, 152),
                                    crgb(171, 221, 164),
                                    crgb(102, 194, 165),
                                    crgb(50, 136, 189),
                                    crgb(94, 79, 162)
                                }
                            }
                        }
                    }
                },
                    BrewerType::div,
                    Properties
                {
                        BlindProperties {2, 2, 2, 0, 0, 0, 0, 0, 0},
                        PrintProperties {1, 1, 1, 0, 0, 0, 0, 0, 0},
                        CopyProperties {1, 1, 1, 0, 0, 0, 0, 0, 0},
                        ScreenProperties {1, 1, 2, 0, 0, 0, 0, 0, 0}
                }
            };
            return cb;
        }

        const Colorbrewer&
        palette_diverging_rd_yl_gn()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "RdYlGn",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlGn",
                                  std::array
                                  {
                                    crgb(252, 141, 89),
                                    crgb(255, 255, 191),
                                    crgb(145, 207, 96)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlGn",
                                  std::array
                                  {
                                    crgb(215, 25, 28),
                                    crgb(253, 174, 97),
                                    crgb(166, 217, 106),
                                    crgb(26, 150, 65)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlGn",
                                  std::array
                                  {
                                    crgb(215, 25, 28),
                                    crgb(253, 174, 97),
                                    crgb(255, 255, 191),
                                    crgb(166, 217, 106),
                                    crgb(26, 150, 65)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlGn",
                                  std::array
                                  {
                                    crgb(215, 48, 39),
                                    crgb(252, 141, 89),
                                    crgb(254, 224, 139),
                                    crgb(217, 239, 139),
                                    crgb(145, 207, 96),
                                    crgb(26, 152, 80)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlGn",
                                  std::array
                                  {
                                    crgb(215, 48, 39),
                                    crgb(252, 141, 89),
                                    crgb(254, 224, 139),
                                    crgb(255, 255, 191),
                                    crgb(217, 239, 139),
                                    crgb(145, 207, 96),
                                    crgb(26, 152, 80)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlGn",
                                  std::array
                                  {
                                    crgb(215, 48, 39),
                                    crgb(244, 109, 67),
                                    crgb(253, 174, 97),
                                    crgb(254, 224, 139),
                                    crgb(217, 239, 139),
                                    crgb(166, 217, 106),
                                    crgb(102, 189, 99),
                                    crgb(26, 152, 80)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlGn",
                                  std::array
                                  {
                                    crgb(215, 48, 39),
                                    crgb(244, 109, 67),
                                    crgb(253, 174, 97),
                                    crgb(254, 224, 139),
                                    crgb(255, 255, 191),
                                    crgb(217, 239, 139),
                                    crgb(166, 217, 106),
                                    crgb(102, 189, 99),
                                    crgb(26, 152, 80)
                                  }
                              }
                          }
                         },

{
    10,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlGn",
                                  std::array
                                  {
                                    crgb(165, 0, 38),
                                    crgb(215, 48, 39),
                                    crgb(244, 109, 67),
                                    crgb(253, 174, 97),
                                    crgb(254, 224, 139),
                                    crgb(217, 239, 139),
                                    crgb(166, 217, 106),
                                    crgb(102, 189, 99),
                                    crgb(26, 152, 80),
                                    crgb(0, 104, 55)
                                  }
                              }
                          }
                         },

{
    11,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlGn",
                                  std::array
                                  {
                                    crgb(165, 0, 38),
                                    crgb(215, 48, 39),
                                    crgb(244, 109, 67),
                                    crgb(253, 174, 97),
                                    crgb(254, 224, 139),
                                    crgb(255, 255, 191),
                                    crgb(217, 239, 139),
                                    crgb(166, 217, 106),
                                    crgb(102, 189, 99),
                                    crgb(26, 152, 80),
                                    crgb(0, 104, 55)
                                  }
                              }
                          }
                         }},
                    BrewerType::div,
                    Properties {BlindProperties {2, 2, 2, 0, 0, 0, 0, 0, 0},
                                PrintProperties {1, 1, 1, 2, 0, 0, 0, 0, 0},
                                CopyProperties {0},
                                ScreenProperties {1, 1, 1, 0, 0, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_diverging_rd_bu()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "RdBu",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdBu",
                                  std::array
                                  {
                                    crgb(239, 138, 98),
                                    crgb(247, 247, 247),
                                    crgb(103, 169, 207)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdBu",
                                  std::array
                                  {
                                    crgb(202, 0, 32),
                                    crgb(244, 165, 130),
                                    crgb(146, 197, 222),
                                    crgb(5, 113, 176)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdBu",
                                  std::array
                                  {
                                    crgb(202, 0, 32),
                                    crgb(244, 165, 130),
                                    crgb(247, 247, 247),
                                    crgb(146, 197, 222),
                                    crgb(5, 113, 176)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdBu",
                                  std::array
                                  {
                                    crgb(178, 24, 43),
                                    crgb(239, 138, 98),
                                    crgb(253, 219, 199),
                                    crgb(209, 229, 240),
                                    crgb(103, 169, 207),
                                    crgb(33, 102, 172)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdBu",
                                  std::array
                                  {
                                    crgb(178, 24, 43),
                                    crgb(239, 138, 98),
                                    crgb(253, 219, 199),
                                    crgb(247, 247, 247),
                                    crgb(209, 229, 240),
                                    crgb(103, 169, 207),
                                    crgb(33, 102, 172)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdBu",
                                  std::array
                                  {
                                    crgb(178, 24, 43),
                                    crgb(214, 96, 77),
                                    crgb(244, 165, 130),
                                    crgb(253, 219, 199),
                                    crgb(209, 229, 240),
                                    crgb(146, 197, 222),
                                    crgb(67, 147, 195),
                                    crgb(33, 102, 172)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdBu",
                                  std::array
                                  {
                                    crgb(178, 24, 43),
                                    crgb(214, 96, 77),
                                    crgb(244, 165, 130),
                                    crgb(253, 219, 199),
                                    crgb(247, 247, 247),
                                    crgb(209, 229, 240),
                                    crgb(146, 197, 222),
                                    crgb(67, 147, 195),
                                    crgb(33, 102, 172)
                                  }
                              }
                          }
                         },

{
    10,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdBu",
                                  std::array
                                  {
                                    crgb(103, 0, 31),
                                    crgb(178, 24, 43),
                                    crgb(214, 96, 77),
                                    crgb(244, 165, 130),
                                    crgb(253, 219, 199),
                                    crgb(209, 229, 240),
                                    crgb(146, 197, 222),
                                    crgb(67, 147, 195),
                                    crgb(33, 102, 172),
                                    crgb(5, 48, 97)
                                  }
                              }
                          }
                         },

{
    11,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdBu",
                                  std::array
                                  {
                                    crgb(103, 0, 31),
                                    crgb(178, 24, 43),
                                    crgb(214, 96, 77),
                                    crgb(244, 165, 130),
                                    crgb(253, 219, 199),
                                    crgb(247, 247, 247),
                                    crgb(209, 229, 240),
                                    crgb(146, 197, 222),
                                    crgb(67, 147, 195),
                                    crgb(33, 102, 172),
                                    crgb(5, 48, 97)
                                  }
                              }
                          }
                         }},
                    BrewerType::div,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 1, 1, 0, 0, 0, 0, 0},
                                CopyProperties {0},
                                ScreenProperties {1, 1, 1, 0, 0, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_diverging_pi_y_g()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "PiYG",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PiYG",
                                  std::array
                                  {
                                    crgb(233, 163, 201),
                                    crgb(247, 247, 247),
                                    crgb(161, 215, 106)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PiYG",
                                  std::array
                                  {
                                    crgb(208, 28, 139),
                                    crgb(241, 182, 218),
                                    crgb(184, 225, 134),
                                    crgb(77, 172, 38)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PiYG",
                                  std::array
                                  {
                                    crgb(208, 28, 139),
                                    crgb(241, 182, 218),
                                    crgb(247, 247, 247),
                                    crgb(184, 225, 134),
                                    crgb(77, 172, 38)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PiYG",
                                  std::array
                                  {
                                    crgb(197, 27, 125),
                                    crgb(233, 163, 201),
                                    crgb(253, 224, 239),
                                    crgb(230, 245, 208),
                                    crgb(161, 215, 106),
                                    crgb(77, 146, 33)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PiYG",
                                  std::array
                                  {
                                    crgb(197, 27, 125),
                                    crgb(233, 163, 201),
                                    crgb(253, 224, 239),
                                    crgb(247, 247, 247),
                                    crgb(230, 245, 208),
                                    crgb(161, 215, 106),
                                    crgb(77, 146, 33)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PiYG",
                                  std::array
                                  {
                                    crgb(197, 27, 125),
                                    crgb(222, 119, 174),
                                    crgb(241, 182, 218),
                                    crgb(253, 224, 239),
                                    crgb(230, 245, 208),
                                    crgb(184, 225, 134),
                                    crgb(127, 188, 65),
                                    crgb(77, 146, 33)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PiYG",
                                  std::array
                                  {
                                    crgb(197, 27, 125),
                                    crgb(222, 119, 174),
                                    crgb(241, 182, 218),
                                    crgb(253, 224, 239),
                                    crgb(247, 247, 247),
                                    crgb(230, 245, 208),
                                    crgb(184, 225, 134),
                                    crgb(127, 188, 65),
                                    crgb(77, 146, 33)
                                  }
                              }
                          }
                         },

{
    10,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PiYG",
                                  std::array
                                  {
                                    crgb(142, 1, 82),
                                    crgb(197, 27, 125),
                                    crgb(222, 119, 174),
                                    crgb(241, 182, 218),
                                    crgb(253, 224, 239),
                                    crgb(230, 245, 208),
                                    crgb(184, 225, 134),
                                    crgb(127, 188, 65),
                                    crgb(77, 146, 33),
                                    crgb(39, 100, 25)
                                  }
                              }
                          }
                         },

{
    11,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PiYG",
                                  std::array
                                  {
                                    crgb(142, 1, 82),
                                    crgb(197, 27, 125),
                                    crgb(222, 119, 174),
                                    crgb(241, 182, 218),
                                    crgb(253, 224, 239),
                                    crgb(247, 247, 247),
                                    crgb(230, 245, 208),
                                    crgb(184, 225, 134),
                                    crgb(127, 188, 65),
                                    crgb(77, 146, 33),
                                    crgb(39, 100, 25)
                                  }
                              }
                          }
                         }},
                    BrewerType::div,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 2, 0, 0, 0, 0, 0, 0},
                                CopyProperties {0},
                                ScreenProperties {1, 1, 2, 0, 0, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_diverging_p_r_gn()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "PRGn",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PRGn",
                                  std::array
                                  {
                                    crgb(175, 141, 195),
                                    crgb(247, 247, 247),
                                    crgb(127, 191, 123)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PRGn",
                                  std::array
                                  {
                                    crgb(123, 50, 148),
                                    crgb(194, 165, 207),
                                    crgb(166, 219, 160),
                                    crgb(0, 136, 55)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PRGn",
                                  std::array
                                  {
                                    crgb(123, 50, 148),
                                    crgb(194, 165, 207),
                                    crgb(247, 247, 247),
                                    crgb(166, 219, 160),
                                    crgb(0, 136, 55)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PRGn",
                                  std::array
                                  {
                                    crgb(118, 42, 131),
                                    crgb(175, 141, 195),
                                    crgb(231, 212, 232),
                                    crgb(217, 240, 211),
                                    crgb(127, 191, 123),
                                    crgb(27, 120, 55)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PRGn",
                                  std::array
                                  {
                                    crgb(118, 42, 131),
                                    crgb(175, 141, 195),
                                    crgb(231, 212, 232),
                                    crgb(247, 247, 247),
                                    crgb(217, 240, 211),
                                    crgb(127, 191, 123),
                                    crgb(27, 120, 55)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PRGn",
                                  std::array
                                  {
                                    crgb(118, 42, 131),
                                    crgb(153, 112, 171),
                                    crgb(194, 165, 207),
                                    crgb(231, 212, 232),
                                    crgb(217, 240, 211),
                                    crgb(166, 219, 160),
                                    crgb(90, 174, 97),
                                    crgb(27, 120, 55)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PRGn",
                                  std::array
                                  {
                                    crgb(118, 42, 131),
                                    crgb(153, 112, 171),
                                    crgb(194, 165, 207),
                                    crgb(231, 212, 232),
                                    crgb(247, 247, 247),
                                    crgb(217, 240, 211),
                                    crgb(166, 219, 160),
                                    crgb(90, 174, 97),
                                    crgb(27, 120, 55)
                                  }
                              }
                          }
                         },

{
    10,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PRGn",
                                  std::array
                                  {
                                    crgb(64, 0, 75),
                                    crgb(118, 42, 131),
                                    crgb(153, 112, 171),
                                    crgb(194, 165, 207),
                                    crgb(231, 212, 232),
                                    crgb(217, 240, 211),
                                    crgb(166, 219, 160),
                                    crgb(90, 174, 97),
                                    crgb(27, 120, 55),
                                    crgb(0, 68, 27)
                                  }
                              }
                          }
                         },

{
    11,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PRGn",
                                  std::array
                                  {
                                    crgb(64, 0, 75),
                                    crgb(118, 42, 131),
                                    crgb(153, 112, 171),
                                    crgb(194, 165, 207),
                                    crgb(231, 212, 232),
                                    crgb(247, 247, 247),
                                    crgb(217, 240, 211),
                                    crgb(166, 219, 160),
                                    crgb(90, 174, 97),
                                    crgb(27, 120, 55),
                                    crgb(0, 68, 27)
                                  }
                              }
                          }
                         }},
                    BrewerType::div,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 1, 1, 0, 0, 0, 0, 0},
                                CopyProperties {0},
                                ScreenProperties {1, 1, 2, 2, 0, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_diverging_rd_yl_bu()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "RdYlBu",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlBu",
                                  std::array
                                  {
                                    crgb(252, 141, 89),
                                    crgb(255, 255, 191),
                                    crgb(145, 191, 219)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlBu",
                                  std::array
                                  {
                                    crgb(215, 25, 28),
                                    crgb(253, 174, 97),
                                    crgb(171, 217, 233),
                                    crgb(44, 123, 182)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlBu",
                                  std::array
                                  {
                                    crgb(215, 25, 28),
                                    crgb(253, 174, 97),
                                    crgb(255, 255, 191),
                                    crgb(171, 217, 233),
                                    crgb(44, 123, 182)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlBu",
                                  std::array
                                  {
                                    crgb(215, 48, 39),
                                    crgb(252, 141, 89),
                                    crgb(254, 224, 144),
                                    crgb(224, 243, 248),
                                    crgb(145, 191, 219),
                                    crgb(69, 117, 180)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlBu",
                                  std::array
                                  {
                                    crgb(215, 48, 39),
                                    crgb(252, 141, 89),
                                    crgb(254, 224, 144),
                                    crgb(255, 255, 191),
                                    crgb(224, 243, 248),
                                    crgb(145, 191, 219),
                                    crgb(69, 117, 180)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlBu",
                                  std::array
                                  {
                                    crgb(215, 48, 39),
                                    crgb(244, 109, 67),
                                    crgb(253, 174, 97),
                                    crgb(254, 224, 144),
                                    crgb(224, 243, 248),
                                    crgb(171, 217, 233),
                                    crgb(116, 173, 209),
                                    crgb(69, 117, 180)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlBu",
                                  std::array
                                  {
                                    crgb(215, 48, 39),
                                    crgb(244, 109, 67),
                                    crgb(253, 174, 97),
                                    crgb(254, 224, 144),
                                    crgb(255, 255, 191),
                                    crgb(224, 243, 248),
                                    crgb(171, 217, 233),
                                    crgb(116, 173, 209),
                                    crgb(69, 117, 180)
                                  }
                              }
                          }
                         },

{
    10,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlBu",
                                  std::array
                                  {
                                    crgb(165, 0, 38),
                                    crgb(215, 48, 39),
                                    crgb(244, 109, 67),
                                    crgb(253, 174, 97),
                                    crgb(254, 224, 144),
                                    crgb(224, 243, 248),
                                    crgb(171, 217, 233),
                                    crgb(116, 173, 209),
                                    crgb(69, 117, 180),
                                    crgb(49, 54, 149)
                                  }
                              }
                          }
                         },

{
    11,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdYlBu",
                                  std::array
                                  {
                                    crgb(165, 0, 38),
                                    crgb(215, 48, 39),
                                    crgb(244, 109, 67),
                                    crgb(253, 174, 97),
                                    crgb(254, 224, 144),
                                    crgb(255, 255, 191),
                                    crgb(224, 243, 248),
                                    crgb(171, 217, 233),
                                    crgb(116, 173, 209),
                                    crgb(69, 117, 180),
                                    crgb(49, 54, 149)
                                  }
                              }
                          }
                         }},
                    BrewerType::div,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 1, 1, 2, 0, 0, 0, 0},
                                CopyProperties {0},
                                ScreenProperties {1, 1, 1, 2, 0, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_diverging_br_b_g()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "BrBG",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BrBG",
                                  std::array
                                  {
                                    crgb(216, 179, 101),
                                    crgb(245, 245, 245),
                                    crgb(90, 180, 172)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BrBG",
                                  std::array
                                  {
                                    crgb(166, 97, 26),
                                    crgb(223, 194, 125),
                                    crgb(128, 205, 193),
                                    crgb(1, 133, 113)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BrBG",
                                  std::array
                                  {
                                    crgb(166, 97, 26),
                                    crgb(223, 194, 125),
                                    crgb(245, 245, 245),
                                    crgb(128, 205, 193),
                                    crgb(1, 133, 113)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BrBG",
                                  std::array
                                  {
                                    crgb(140, 81, 10),
                                    crgb(216, 179, 101),
                                    crgb(246, 232, 195),
                                    crgb(199, 234, 229),
                                    crgb(90, 180, 172),
                                    crgb(1, 102, 94)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BrBG",
                                  std::array
                                  {
                                    crgb(140, 81, 10),
                                    crgb(216, 179, 101),
                                    crgb(246, 232, 195),
                                    crgb(245, 245, 245),
                                    crgb(199, 234, 229),
                                    crgb(90, 180, 172),
                                    crgb(1, 102, 94)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BrBG",
                                  std::array
                                  {
                                    crgb(140, 81, 10),
                                    crgb(191, 129, 45),
                                    crgb(223, 194, 125),
                                    crgb(246, 232, 195),
                                    crgb(199, 234, 229),
                                    crgb(128, 205, 193),
                                    crgb(53, 151, 143),
                                    crgb(1, 102, 94)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BrBG",
                                  std::array
                                  {
                                    crgb(140, 81, 10),
                                    crgb(191, 129, 45),
                                    crgb(223, 194, 125),
                                    crgb(246, 232, 195),
                                    crgb(245, 245, 245),
                                    crgb(199, 234, 229),
                                    crgb(128, 205, 193),
                                    crgb(53, 151, 143),
                                    crgb(1, 102, 94)
                                  }
                              }
                          }
                         },

{
    10,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BrBG",
                                  std::array
                                  {
                                    crgb(84, 48, 5),
                                    crgb(140, 81, 10),
                                    crgb(191, 129, 45),
                                    crgb(223, 194, 125),
                                    crgb(246, 232, 195),
                                    crgb(199, 234, 229),
                                    crgb(128, 205, 193),
                                    crgb(53, 151, 143),
                                    crgb(1, 102, 94),
                                    crgb(0, 60, 48)
                                  }
                              }
                          }
                         },

{
    11,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BrBG",
                                  std::array
                                  {
                                    crgb(84, 48, 5),
                                    crgb(140, 81, 10),
                                    crgb(191, 129, 45),
                                    crgb(223, 194, 125),
                                    crgb(246, 232, 195),
                                    crgb(245, 245, 245),
                                    crgb(199, 234, 229),
                                    crgb(128, 205, 193),
                                    crgb(53, 151, 143),
                                    crgb(1, 102, 94),
                                    crgb(0, 60, 48)
                                  }
                              }
                          }
                         }},
                    BrewerType::div,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 1, 1, 0, 0, 0, 0, 0},
                                CopyProperties {0},
                                ScreenProperties {1, 1, 1, 1, 0, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_diverging_rd_gy()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "RdGy",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(239, 138, 98),
                                    crgb(255, 255, 255),
                                    crgb(153, 153, 153)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(202, 0, 32),
                                    crgb(244, 165, 130),
                                    crgb(186, 186, 186),
                                    crgb(64, 64, 64)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(202, 0, 32),
                                    crgb(244, 165, 130),
                                    crgb(255, 255, 255),
                                    crgb(186, 186, 186),
                                    crgb(64, 64, 64)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(178, 24, 43),
                                    crgb(239, 138, 98),
                                    crgb(253, 219, 199),
                                    crgb(224, 224, 224),
                                    crgb(153, 153, 153),
                                    crgb(77, 77, 77)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(178, 24, 43),
                                    crgb(239, 138, 98),
                                    crgb(253, 219, 199),
                                    crgb(255, 255, 255),
                                    crgb(224, 224, 224),
                                    crgb(153, 153, 153),
                                    crgb(77, 77, 77)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(178, 24, 43),
                                    crgb(214, 96, 77),
                                    crgb(244, 165, 130),
                                    crgb(253, 219, 199),
                                    crgb(224, 224, 224),
                                    crgb(186, 186, 186),
                                    crgb(135, 135, 135),
                                    crgb(77, 77, 77)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(178, 24, 43),
                                    crgb(214, 96, 77),
                                    crgb(244, 165, 130),
                                    crgb(253, 219, 199),
                                    crgb(255, 255, 255),
                                    crgb(224, 224, 224),
                                    crgb(186, 186, 186),
                                    crgb(135, 135, 135),
                                    crgb(77, 77, 77)
                                  }
                              }
                          }
                         },

{
    10,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(103, 0, 31),
                                    crgb(178, 24, 43),
                                    crgb(214, 96, 77),
                                    crgb(244, 165, 130),
                                    crgb(253, 219, 199),
                                    crgb(224, 224, 224),
                                    crgb(186, 186, 186),
                                    crgb(135, 135, 135),
                                    crgb(77, 77, 77),
                                    crgb(26, 26, 26)
                                  }
                              }
                          }
                         },

{
    11,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(103, 0, 31),
                                    crgb(178, 24, 43),
                                    crgb(214, 96, 77),
                                    crgb(244, 165, 130),
                                    crgb(253, 219, 199),
                                    crgb(255, 255, 255),
                                    crgb(224, 224, 224),
                                    crgb(186, 186, 186),
                                    crgb(135, 135, 135),
                                    crgb(77, 77, 77),
                                    crgb(26, 26, 26)
                                  }
                              }
                          }
                         }},
                    BrewerType::div,
                    Properties {BlindProperties {2},
                                PrintProperties {1, 1, 1, 2, 0, 0, 0, 0, 0},
                                CopyProperties {0},
                                ScreenProperties {1, 1, 2, 0, 0, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_diverging_pu_or()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "PuOr",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(241, 163, 64),
                                    crgb(247, 247, 247),
                                    crgb(153, 142, 195)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(230, 97, 1),
                                    crgb(253, 184, 99),
                                    crgb(178, 171, 210),
                                    crgb(94, 60, 153)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(230, 97, 1),
                                    crgb(253, 184, 99),
                                    crgb(247, 247, 247),
                                    crgb(178, 171, 210),
                                    crgb(94, 60, 153)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(179, 88, 6),
                                    crgb(241, 163, 64),
                                    crgb(254, 224, 182),
                                    crgb(216, 218, 235),
                                    crgb(153, 142, 195),
                                    crgb(84, 39, 136)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(179, 88, 6),
                                    crgb(241, 163, 64),
                                    crgb(254, 224, 182),
                                    crgb(247, 247, 247),
                                    crgb(216, 218, 235),
                                    crgb(153, 142, 195),
                                    crgb(84, 39, 136)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(179, 88, 6),
                                    crgb(224, 130, 20),
                                    crgb(253, 184, 99),
                                    crgb(254, 224, 182),
                                    crgb(216, 218, 235),
                                    crgb(178, 171, 210),
                                    crgb(128, 115, 172),
                                    crgb(84, 39, 136)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(179, 88, 6),
                                    crgb(224, 130, 20),
                                    crgb(253, 184, 99),
                                    crgb(254, 224, 182),
                                    crgb(247, 247, 247),
                                    crgb(216, 218, 235),
                                    crgb(178, 171, 210),
                                    crgb(128, 115, 172),
                                    crgb(84, 39, 136)
                                  }
                              }
                          }
                         },

{
    10,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(127, 59, 8),
                                    crgb(179, 88, 6),
                                    crgb(224, 130, 20),
                                    crgb(253, 184, 99),
                                    crgb(254, 224, 182),
                                    crgb(216, 218, 235),
                                    crgb(178, 171, 210),
                                    crgb(128, 115, 172),
                                    crgb(84, 39, 136),
                                    crgb(45, 0, 75)
                                  }
                              }
                          }
                         },

{
    11,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdGy",
                                  std::array
                                  {
                                    crgb(127, 59, 8),
                                    crgb(179, 88, 6),
                                    crgb(224, 130, 20),
                                    crgb(253, 184, 99),
                                    crgb(254, 224, 182),
                                    crgb(247, 247, 247),
                                    crgb(216, 218, 235),
                                    crgb(178, 171, 210),
                                    crgb(128, 115, 172),
                                    crgb(84, 39, 136),
                                    crgb(45, 0, 75)
                                  }
                              }
                          }
                         }},
                    BrewerType::div,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 2, 2, 0, 0, 0, 0, 0},
                                CopyProperties {1, 1, 0, 0, 0, 0, 0, 0, 0},
                                ScreenProperties {1, 1, 1, 1, 0, 0, 0, 0, 0}}};
            return cb;
        }


        /*** Qualitative ***/
        const Colorbrewer&
        palette_qualitative_set2()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "Set2",
                    SizeToPaletteMap {

                    {
                        3,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Set2",
                                     std::array
                                     {
                                        crgb(102, 194, 165),
                                        crgb(252, 141, 98),
                                        crgb(141, 160, 203)
                                     }
                                 }
                             }
                            },

                {
                    4,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Set2",
                                     std::array
                                     {
                                        crgb(102, 194, 165),
                                        crgb(252, 141, 98),
                                        crgb(141, 160, 203),
                                        crgb(231, 138, 195)
                                     }
                                 }
                             }
                            },

            {
                5,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Set2",
                                     std::array
                                     {
                                        crgb(102, 194, 165),
                                        crgb(252, 141, 98),
                                        crgb(141, 160, 203),
                                        crgb(231, 138, 195),
                                        crgb(166, 216, 84)
                                     }
                                 }
                             }
                            },

        {
            6,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Set2",
                                     std::array
                                     {
                                        crgb(102, 194, 165),
                                        crgb(252, 141, 98),
                                        crgb(141, 160, 203),
                                        crgb(231, 138, 195),
                                        crgb(166, 216, 84),
                                        crgb(255, 217, 47)
                                     }
                                 }
                             }
                            },

    {
        7,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Set2",
                                     std::array
                                     {
                                        crgb(102, 194, 165),
                                        crgb(252, 141, 98),
                                        crgb(141, 160, 203),
                                        crgb(231, 138, 195),
                                        crgb(166, 216, 84),
                                        crgb(255, 217, 47),
                                        crgb(229, 196, 148)
                                     }
                                 }
                             }
                            },

{
    8,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Set2",
                                     std::array
                                     {
                                        crgb(102, 194, 165),
                                        crgb(252, 141, 98),
                                        crgb(141, 160, 203),
                                        crgb(231, 138, 195),
                                        crgb(166, 216, 84),
                                        crgb(255, 217, 47),
                                        crgb(229, 196, 148),
                                        crgb(179, 179, 179)
                                     }
                                 }
                             }
                            },
                    },
                    BrewerType::qual,
                    Properties {BlindProperties {1, 2, 2, 2, 0, 0, 0},
                                PrintProperties {1, 1, 1, 2, 2, 2},
                                CopyProperties {0},
                                ScreenProperties {1, 1, 2, 2, 2, 2}}};
            return cb;
        }

        const Colorbrewer&
        palette_qualitative_accent()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "Accent",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Accent",
                                  std::array
                                  {
                                    crgb(127, 201, 127),
                                    crgb(190, 174, 212),
                                    crgb(253, 192, 134)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Accent",
                                  std::array
                                  {
                                    crgb(127, 201, 127),
                                    crgb(190, 174, 212),
                                    crgb(253, 192, 134),
                                    crgb(255, 255, 153)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Accent",
                                  std::array
                                  {
                                    crgb(127, 201, 127),
                                    crgb(190, 174, 212),
                                    crgb(253, 192, 134),
                                    crgb(255, 255, 153),
                                    crgb(56, 108, 176)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Accent",
                                  std::array
                                  {
                                    crgb(127, 201, 127),
                                    crgb(190, 174, 212),
                                    crgb(253, 192, 134),
                                    crgb(255, 255, 153),
                                    crgb(56, 108, 176),
                                    crgb(240, 2, 127)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Accent",
                                  std::array
                                  {
                                    crgb(127, 201, 127),
                                    crgb(190, 174, 212),
                                    crgb(253, 192, 134),
                                    crgb(255, 255, 153),
                                    crgb(56, 108, 176),
                                    crgb(240, 2, 127),
                                    crgb(191, 91, 23)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Accent",
                                  std::array
                                  {
                                    crgb(127, 201, 127),
                                    crgb(190, 174, 212),
                                    crgb(253, 192, 134),
                                    crgb(255, 255, 153),
                                    crgb(56, 108, 176),
                                    crgb(240, 2, 127),
                                    crgb(191, 91, 23),
                                    crgb(102, 102, 102)
                                  }
                              }
                          }
                         }},
                    BrewerType::qual,
                    Properties {BlindProperties {2, 0, 0, 0, 0, 0, 0},
                                PrintProperties {1, 1, 2, 2, 2, 2},
                                CopyProperties {0},
                                ScreenProperties {1, 1, 1, 2, 2, 2}}};
            return cb;
        }

        const Colorbrewer&
        palette_qualitative_set1()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "Set1",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set1",
                                  std::array
                                  {
                                    crgb(228, 26, 28),
                                    crgb(55, 126, 184),
                                    crgb(77, 175, 74)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set1",
                                  std::array
                                  {
                                    crgb(228, 26, 28),
                                    crgb(55, 126, 184),
                                    crgb(77, 175, 74),
                                    crgb(152, 78, 163)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set1",
                                  std::array
                                  {
                                    crgb(228, 26, 28),
                                    crgb(55, 126, 184),
                                    crgb(77, 175, 74),
                                    crgb(152, 78, 163),
                                    crgb(255, 127, 0)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set1",
                                  std::array
                                  {
                                    crgb(228, 26, 28),
                                    crgb(55, 126, 184),
                                    crgb(77, 175, 74),
                                    crgb(152, 78, 163),
                                    crgb(255, 127, 0),
                                    crgb(255, 255, 51)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set1",
                                  std::array
                                  {
                                    crgb(228, 26, 28),
                                    crgb(55, 126, 184),
                                    crgb(77, 175, 74),
                                    crgb(152, 78, 163),
                                    crgb(255, 127, 0),
                                    crgb(255, 255, 51),
                                    crgb(166, 86, 40)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set1",
                                  std::array
                                  {
                                    crgb(228, 26, 28),
                                    crgb(55, 126, 184),
                                    crgb(77, 175, 74),
                                    crgb(152, 78, 163),
                                    crgb(255, 127, 0),
                                    crgb(255, 255, 51),
                                    crgb(166, 86, 40),
                                    crgb(247, 129, 191)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set1",
                                  std::array
                                  {
                                    crgb(228, 26, 28),
                                    crgb(55, 126, 184),
                                    crgb(77, 175, 74),
                                    crgb(152, 78, 163),
                                    crgb(255, 127, 0),
                                    crgb(255, 255, 51),
                                    crgb(166, 86, 40),
                                    crgb(247, 129, 191),
                                    crgb(153, 153, 153)
                                  }
                              }
                          }
                         }},
                    BrewerType::qual,
                    Properties {BlindProperties {2}, PrintProperties {1}, CopyProperties {0}, ScreenProperties {1}}};
            return cb;
        }

        const Colorbrewer&
        palette_qualitative_set3()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "Set3",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set3",
                                  std::array
                                  {
                                    crgb(141, 211, 199),
                                    crgb(255, 255, 179),
                                    crgb(190, 186, 218)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set3",
                                  std::array
                                  {
                                    crgb(141, 211, 199),
                                    crgb(255, 255, 179),
                                    crgb(190, 186, 218),
                                    crgb(251, 128, 114)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set3",
                                  std::array
                                  {
                                    crgb(141, 211, 199),
                                    crgb(255, 255, 179),
                                    crgb(190, 186, 218),
                                    crgb(251, 128, 114),
                                    crgb(128, 177, 211)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set3",
                                  std::array
                                  {
                                    crgb(141, 211, 199),
                                    crgb(255, 255, 179),
                                    crgb(190, 186, 218),
                                    crgb(251, 128, 114),
                                    crgb(128, 177, 211),
                                    crgb(253, 180, 98)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set3",
                                  std::array
                                  {
                                    crgb(141, 211, 199),
                                    crgb(255, 255, 179),
                                    crgb(190, 186, 218),
                                    crgb(251, 128, 114),
                                    crgb(128, 177, 211),
                                    crgb(253, 180, 98),
                                    crgb(179, 222, 105)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set3",
                                  std::array
                                  {
                                    crgb(141, 211, 199),
                                    crgb(255, 255, 179),
                                    crgb(190, 186, 218),
                                    crgb(251, 128, 114),
                                    crgb(128, 177, 211),
                                    crgb(253, 180, 98),
                                    crgb(179, 222, 105),
                                    crgb(252, 205, 229)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set3",
                                  std::array
                                  {
                                    crgb(141, 211, 199),
                                    crgb(255, 255, 179),
                                    crgb(190, 186, 218),
                                    crgb(251, 128, 114),
                                    crgb(128, 177, 211),
                                    crgb(253, 180, 98),
                                    crgb(179, 222, 105),
                                    crgb(252, 205, 229),
                                    crgb(217, 217, 217)
                                  }
                              }
                          }
                         },

{
    10,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set3",
                                  std::array
                                  {
                                    crgb(141, 211, 199),
                                    crgb(255, 255, 179),
                                    crgb(190, 186, 218),
                                    crgb(251, 128, 114),
                                    crgb(128, 177, 211),
                                    crgb(253, 180, 98),
                                    crgb(179, 222, 105),
                                    crgb(252, 205, 229),
                                    crgb(217, 217, 217),
                                    crgb(188, 128, 189)
                                  }
                              }
                          }
                         },

{
    11,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set3",
                                  std::array
                                  {
                                    crgb(141, 211, 199),
                                    crgb(255, 255, 179),
                                    crgb(190, 186, 218),
                                    crgb(251, 128, 114),
                                    crgb(128, 177, 211),
                                    crgb(253, 180, 98),
                                    crgb(179, 222, 105),
                                    crgb(252, 205, 229),
                                    crgb(217, 217, 217),
                                    crgb(188, 128, 189),
                                    crgb(204, 235, 197)
                                  }
                              }
                          }
                         },

{
    12,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Set3",
                                  std::array
                                  {
                                    crgb(141, 211, 199),
                                    crgb(255, 255, 179),
                                    crgb(190, 186, 218),
                                    crgb(251, 128, 114),
                                    crgb(128, 177, 211),
                                    crgb(253, 180, 98),
                                    crgb(179, 222, 105),
                                    crgb(252, 205, 229),
                                    crgb(217, 217, 217),
                                    crgb(188, 128, 189),
                                    crgb(204, 235, 197),
                                    crgb(255, 237, 111)
                                  }
                              }
                          }
                         }},
                    BrewerType::qual,
                    Properties {BlindProperties {2, 2, 0, 0, 0, 0, 0, 0, 0, 0},
                                PrintProperties {1, 1, 1, 1, 1, 1, 2, 0, 0, 0},
                                CopyProperties {1, 2, 2, 2, 2, 2, 2, 0, 0, 0},
                                ScreenProperties {1, 1, 1, 2, 2, 2, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_qualitative_dark2()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "Dark2",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Dark2",
                                  std::array
                                  {
                                    crgb(27, 158, 119),
                                    crgb(217, 95, 2),
                                    crgb(117, 112, 179)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Dark2",
                                  std::array
                                  {
                                    crgb(27, 158, 119),
                                    crgb(217, 95, 2),
                                    crgb(117, 112, 179),
                                    crgb(231, 41, 138)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Dark2",
                                  std::array
                                  {
                                    crgb(27, 158, 119),
                                    crgb(217, 95, 2),
                                    crgb(117, 112, 179),
                                    crgb(231, 41, 138),
                                    crgb(102, 166, 30)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Dark2",
                                  std::array
                                  {
                                    crgb(27, 158, 119),
                                    crgb(217, 95, 2),
                                    crgb(117, 112, 179),
                                    crgb(231, 41, 138),
                                    crgb(102, 166, 30),
                                    crgb(230, 171, 2)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Dark2",
                                  std::array
                                  {
                                    crgb(27, 158, 119),
                                    crgb(217, 95, 2),
                                    crgb(117, 112, 179),
                                    crgb(231, 41, 138),
                                    crgb(102, 166, 30),
                                    crgb(230, 171, 2),
                                    crgb(166, 118, 29)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Dark2",
                                  std::array
                                  {
                                    crgb(27, 158, 119),
                                    crgb(217, 95, 2),
                                    crgb(117, 112, 179),
                                    crgb(231, 41, 138),
                                    crgb(102, 166, 30),
                                    crgb(230, 171, 2),
                                    crgb(166, 118, 29),
                                    crgb(102, 102, 102)
                                  }
                              }
                          }
                         }},
                    BrewerType::qual,
                    Properties {BlindProperties {1, 2, 2, 2, 0, 0},
                                PrintProperties {1},
                                CopyProperties {0},
                                ScreenProperties {1}}};
            return cb;
        }

        const Colorbrewer&
        palette_qualitative_paired()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "Paired",
                    SizeToPaletteMap {

                    {
                        3,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Paired",
                                     std::array
                                     {
                                        crgb(166, 206, 227),
                                        crgb(31, 120, 180),
                                        crgb(178, 223, 138)
                                     }
                                 }
                             }
                            },

                {
                    4,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Paired",
                                     std::array
                                     {
                                        crgb(166, 206, 227),
                                        crgb(31, 120, 180),
                                        crgb(178, 223, 138),
                                        crgb(51, 160, 44)
                                     }
                                 }
                             }
                            },

            {
                5,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Paired",
                                     std::array
                                     {
                                        crgb(166, 206, 227),
                                        crgb(31, 120, 180),
                                        crgb(178, 223, 138),
                                        crgb(51, 160, 44),
                                        crgb(251, 154, 153)
                                     }
                                 }
                             }
                            },

        {
            6,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Paired",
                                     std::array
                                     {
                                        crgb(166, 206, 227),
                                        crgb(31, 120, 180),
                                        crgb(178, 223, 138),
                                        crgb(51, 160, 44),
                                        crgb(251, 154, 153),
                                        crgb(227, 26, 28)
                                     }
                                 }
                             }
                            },

    {
        7,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Paired",
                                     std::array
                                     {
                                        crgb(166, 206, 227),
                                        crgb(31, 120, 180),
                                        crgb(178, 223, 138),
                                        crgb(51, 160, 44),
                                        crgb(251, 154, 153),
                                        crgb(227, 26, 28),
                                        crgb(253, 191, 111)
                                     }
                                 }
                             }
                            },

{
    8,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Paired",
                                     std::array
                                     {
                                        crgb(166, 206, 227),
                                        crgb(31, 120, 180),
                                        crgb(178, 223, 138),
                                        crgb(51, 160, 44),
                                        crgb(251, 154, 153),
                                        crgb(227, 26, 28),
                                        crgb(253, 191, 111),
                                        crgb(255, 127, 0)
                                     }
                                 }
                             }
                            },

{
    9,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Paired",
                                     std::array
                                     {
                                        crgb(166, 206, 227),
                                        crgb(31, 120, 180),
                                        crgb(178, 223, 138),
                                        crgb(51, 160, 44),
                                        crgb(251, 154, 153),
                                        crgb(227, 26, 28),
                                        crgb(253, 191, 111),
                                        crgb(255, 127, 0),
                                        crgb(202, 178, 214)
                                     }
                                 }
                             }
                            },

{
    10,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Paired",
                                     std::array
                                     {
                                        crgb(166, 206, 227),
                                        crgb(31, 120, 180),
                                        crgb(178, 223, 138),
                                        crgb(51, 160, 44),
                                        crgb(251, 154, 153),
                                        crgb(227, 26, 28),
                                        crgb(253, 191, 111),
                                        crgb(255, 127, 0),
                                        crgb(202, 178, 214),
                                        crgb(106, 61, 154)
                                     }
                                 }
                             }
                            },

{
    11,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Paired",
                                     std::array
                                     {
                                        crgb(166, 206, 227),
                                        crgb(31, 120, 180),
                                        crgb(178, 223, 138),
                                        crgb(51, 160, 44),
                                        crgb(251, 154, 153),
                                        crgb(227, 26, 28),
                                        crgb(253, 191, 111),
                                        crgb(255, 127, 0),
                                        crgb(202, 178, 214),
                                        crgb(106, 61, 154),
                                        crgb(255, 255, 153)
                                     }
                                 }
                             }
                            },

{
    12,
                             BrewerPalette
                             {
                                 Palette
                                 {
                                     "Paired",
                                     std::array
                                     {
                                        crgb(166, 206, 227),
                                        crgb(31, 120, 180),
                                        crgb(178, 223, 138),
                                        crgb(51, 160, 44),
                                        crgb(251, 154, 153),
                                        crgb(227, 26, 28),
                                        crgb(253, 191, 111),
                                        crgb(255, 127, 0),
                                        crgb(202, 178, 214),
                                        crgb(106, 61, 154),
                                        crgb(255, 255, 153),
                                        crgb(177, 89, 40)
                                     }
                                 }
                             }
                            },
                    },
                    BrewerType::qual,
                    Properties {BlindProperties {1, 1, 2, 2, 2, 2, 0, 0, 0},
                                PrintProperties {1, 1, 1, 1, 1, 2, 2, 2, 2},
                                CopyProperties {0},
                                ScreenProperties {1, 1, 1, 1, 1, 1, 1, 1, 2}}};
            return cb;
        }

        const Colorbrewer&
        palette_qualitative_pastel2()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "Pastel2",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Pastel2",
                                  std::array
                                  {
                                    crgb(179, 226, 205),
                                    crgb(253, 205, 172),
                                    crgb(203, 213, 232)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Pastel2",
                                  std::array
                                  {
                                    crgb(179, 226, 205),
                                    crgb(253, 205, 172),
                                    crgb(203, 213, 232),
                                    crgb(244, 202, 228)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Pastel2",
                                  std::array
                                  {
                                    crgb(179, 226, 205),
                                    crgb(253, 205, 172),
                                    crgb(203, 213, 232),
                                    crgb(244, 202, 228),
                                    crgb(230, 245, 201)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Pastel2",
                                  std::array
                                  {
                                    crgb(179, 226, 205),
                                    crgb(253, 205, 172),
                                    crgb(203, 213, 232),
                                    crgb(244, 202, 228),
                                    crgb(230, 245, 201),
                                    crgb(255, 242, 174)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Pastel2",
                                  std::array
                                  {
                                    crgb(179, 226, 205),
                                    crgb(253, 205, 172),
                                    crgb(203, 213, 232),
                                    crgb(244, 202, 228),
                                    crgb(230, 245, 201),
                                    crgb(255, 242, 174),
                                    crgb(241, 226, 204)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Pastel2",
                                  std::array
                                  {
                                    crgb(179, 226, 205),
                                    crgb(253, 205, 172),
                                    crgb(203, 213, 232),
                                    crgb(244, 202, 228),
                                    crgb(230, 245, 201),
                                    crgb(255, 242, 174),
                                    crgb(241, 226, 204),
                                    crgb(204, 204, 204)
                                  }
                              }
                          }
                         }},
                    BrewerType::qual,
                    Properties {BlindProperties {2, 0, 0, 0, 0, 0},
                                PrintProperties {2, 0, 0, 0, 0, 0},
                                CopyProperties {0},
                                ScreenProperties {2, 2, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_qualitative_pastel1()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "Pastel1",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Pastel1",
                                  std::array
                                  {
                                    crgb(251, 180, 174),
                                    crgb(179, 205, 227),
                                    crgb(204, 235, 197)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Pastel1",
                                  std::array
                                  {
                                    crgb(251, 180, 174),
                                    crgb(179, 205, 227),
                                    crgb(204, 235, 197),
                                    crgb(222, 203, 228)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Pastel1",
                                  std::array
                                  {
                                    crgb(251, 180, 174),
                                    crgb(179, 205, 227),
                                    crgb(204, 235, 197),
                                    crgb(222, 203, 228),
                                    crgb(254, 217, 166)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Pastel1",
                                  std::array
                                  {
                                    crgb(251, 180, 174),
                                    crgb(179, 205, 227),
                                    crgb(204, 235, 197),
                                    crgb(222, 203, 228),
                                    crgb(254, 217, 166),
                                    crgb(255, 255, 204)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Pastel1",
                                  std::array
                                  {
                                    crgb(251, 180, 174),
                                    crgb(179, 205, 227),
                                    crgb(204, 235, 197),
                                    crgb(222, 203, 228),
                                    crgb(254, 217, 166),
                                    crgb(255, 255, 204),
                                    crgb(229, 216, 189)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Pastel1",
                                  std::array
                                  {
                                    crgb(251, 180, 174),
                                    crgb(179, 205, 227),
                                    crgb(204, 235, 197),
                                    crgb(222, 203, 228),
                                    crgb(254, 217, 166),
                                    crgb(255, 255, 204),
                                    crgb(229, 216, 189),
                                    crgb(253, 218, 236)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Pastel1",
                                  std::array
                                  {
                                    crgb(251, 180, 174),
                                    crgb(179, 205, 227),
                                    crgb(204, 235, 197),
                                    crgb(222, 203, 228),
                                    crgb(254, 217, 166),
                                    crgb(255, 255, 204),
                                    crgb(229, 216, 189),
                                    crgb(253, 218, 236),
                                    crgb(242, 242, 242)
                                  }
                              }
                          }
                         }},
                    BrewerType::qual,
                    Properties {BlindProperties {2, 0, 0, 0, 0, 0, 0},
                                PrintProperties {2, 2, 2, 0, 0, 0, 0},
                                CopyProperties {0},
                                ScreenProperties {2, 2, 2, 2, 0, 0, 0}}};
            return cb;
        }


        /*** Sequential ***/
        const Colorbrewer&
        palette_sequential_or_rd()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "OrRd",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "OrRd",
                                  std::array
                                  {
                                    crgb(254, 232, 200),
                                    crgb(253, 187, 132),
                                    crgb(227, 74, 51)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "OrRd",
                                  std::array
                                  {
                                    crgb(254, 240, 217),
                                    crgb(253, 204, 138),
                                    crgb(252, 141, 89),
                                    crgb(215, 48, 31)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "OrRd",
                                  std::array
                                  {
                                    crgb(254, 240, 217),
                                    crgb(253, 204, 138),
                                    crgb(252, 141, 89),
                                    crgb(227, 74, 51),
                                    crgb(179, 0, 0)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "OrRd",
                                  std::array
                                  {
                                    crgb(254, 240, 217),
                                    crgb(253, 212, 158),
                                    crgb(253, 187, 132),
                                    crgb(252, 141, 89),
                                    crgb(227, 74, 51),
                                    crgb(179, 0, 0)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "OrRd",
                                  std::array
                                  {
                                    crgb(254, 240, 217),
                                    crgb(253, 212, 158),
                                    crgb(253, 187, 132),
                                    crgb(252, 141, 89),
                                    crgb(239, 101, 72),
                                    crgb(215, 48, 31),
                                    crgb(153, 0, 0)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "OrRd",
                                  std::array
                                  {
                                    crgb(255, 247, 236),
                                    crgb(254, 232, 200),
                                    crgb(253, 212, 158),
                                    crgb(253, 187, 132),
                                    crgb(252, 141, 89),
                                    crgb(239, 101, 72),
                                    crgb(215, 48, 31),
                                    crgb(153, 0, 0)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "OrRd",
                                  std::array
                                  {
                                    crgb(255, 247, 236),
                                    crgb(254, 232, 200),
                                    crgb(253, 212, 158),
                                    crgb(253, 187, 132),
                                    crgb(252, 141, 89),
                                    crgb(239, 101, 72),
                                    crgb(215, 48, 31),
                                    crgb(179, 0, 0),
                                    crgb(127, 0, 0)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 0, 0, 0, 0, 0},
                                CopyProperties {1, 1, 2, 0, 0, 0, 0},
                                ScreenProperties {1, 1, 1, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_pu_bu()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "PuBu",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuBu",
                                  std::array
                                  {
                                    crgb(236, 231, 242),
                                    crgb(166, 189, 219),
                                    crgb(43, 140, 190)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuBu",
                                  std::array
                                  {
                                    crgb(241, 238, 246),
                                    crgb(189, 201, 225),
                                    crgb(116, 169, 207),
                                    crgb(5, 112, 176)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuBu",
                                  std::array
                                  {
                                    crgb(241, 238, 246),
                                    crgb(189, 201, 225),
                                    crgb(116, 169, 207),
                                    crgb(43, 140, 190),
                                    crgb(4, 90, 141)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuBu",
                                  std::array
                                  {
                                    crgb(241, 238, 246),
                                    crgb(208, 209, 230),
                                    crgb(166, 189, 219),
                                    crgb(116, 169, 207),
                                    crgb(43, 140, 190),
                                    crgb(4, 90, 141)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuBu",
                                  std::array
                                  {
                                    crgb(241, 238, 246),
                                    crgb(208, 209, 230),
                                    crgb(166, 189, 219),
                                    crgb(116, 169, 207),
                                    crgb(54, 144, 192),
                                    crgb(5, 112, 176),
                                    crgb(3, 78, 123)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuBu",
                                  std::array
                                  {
                                    crgb(255, 247, 251),
                                    crgb(236, 231, 242),
                                    crgb(208, 209, 230),
                                    crgb(166, 189, 219),
                                    crgb(116, 169, 207),
                                    crgb(54, 144, 192),
                                    crgb(5, 112, 176),
                                    crgb(3, 78, 123)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuBu",
                                  std::array
                                  {
                                    crgb(255, 247, 251),
                                    crgb(236, 231, 242),
                                    crgb(208, 209, 230),
                                    crgb(166, 189, 219),
                                    crgb(116, 169, 207),
                                    crgb(54, 144, 192),
                                    crgb(5, 112, 176),
                                    crgb(4, 90, 141),
                                    crgb(2, 56, 88)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 2, 2, 0, 0, 0, 0},
                                CopyProperties {1, 2, 0, 0, 0, 0, 0},
                                ScreenProperties {1, 1, 2, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_bu_pu()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "BuPu",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BuPu",
                                  std::array
                                  {
                                    crgb(224, 236, 244),
                                    crgb(158, 188, 218),
                                    crgb(136, 86, 167)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BuPu",
                                  std::array
                                  {
                                    crgb(237, 248, 251),
                                    crgb(179, 205, 227),
                                    crgb(140, 150, 198),
                                    crgb(136, 65, 157)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BuPu",
                                  std::array
                                  {
                                    crgb(237, 248, 251),
                                    crgb(179, 205, 227),
                                    crgb(140, 150, 198),
                                    crgb(136, 86, 167),
                                    crgb(129, 15, 124)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BuPu",
                                  std::array
                                  {
                                    crgb(237, 248, 251),
                                    crgb(191, 211, 230),
                                    crgb(158, 188, 218),
                                    crgb(140, 150, 198),
                                    crgb(136, 86, 167),
                                    crgb(129, 15, 124)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BuPu",
                                  std::array
                                  {
                                    crgb(237, 248, 251),
                                    crgb(191, 211, 230),
                                    crgb(158, 188, 218),
                                    crgb(140, 150, 198),
                                    crgb(140, 107, 177),
                                    crgb(136, 65, 157),
                                    crgb(110, 1, 107)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BuPu",
                                  std::array
                                  {
                                    crgb(247, 252, 253),
                                    crgb(224, 236, 244),
                                    crgb(191, 211, 230),
                                    crgb(158, 188, 218),
                                    crgb(140, 150, 198),
                                    crgb(140, 107, 177),
                                    crgb(136, 65, 157),
                                    crgb(110, 1, 107)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BuPu",
                                  std::array
                                  {
                                    crgb(247, 252, 253),
                                    crgb(224, 236, 244),
                                    crgb(191, 211, 230),
                                    crgb(158, 188, 218),
                                    crgb(140, 150, 198),
                                    crgb(140, 107, 177),
                                    crgb(136, 65, 157),
                                    crgb(129, 15, 124),
                                    crgb(77, 0, 75)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 2, 2, 0, 0, 0},
                                CopyProperties {1, 2, 0, 0, 0, 0, 0},
                                ScreenProperties {1, 1, 1, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_oranges()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "Oranges",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Oranges",
                                  std::array
                                  {
                                    crgb(254, 230, 206),
                                    crgb(253, 174, 107),
                                    crgb(230, 85, 13)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Oranges",
                                  std::array
                                  {
                                    crgb(254, 237, 222),
                                    crgb(253, 190, 133),
                                    crgb(253, 141, 60),
                                    crgb(217, 71, 1)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Oranges",
                                  std::array
                                  {
                                    crgb(254, 237, 222),
                                    crgb(253, 190, 133),
                                    crgb(253, 141, 60),
                                    crgb(230, 85, 13),
                                    crgb(166, 54, 3)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Oranges",
                                  std::array
                                  {
                                    crgb(254, 237, 222),
                                    crgb(253, 208, 162),
                                    crgb(253, 174, 107),
                                    crgb(253, 141, 60),
                                    crgb(230, 85, 13),
                                    crgb(166, 54, 3)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Oranges",
                                  std::array
                                  {
                                    crgb(254, 237, 222),
                                    crgb(253, 208, 162),
                                    crgb(253, 174, 107),
                                    crgb(253, 141, 60),
                                    crgb(241, 105, 19),
                                    crgb(217, 72, 1),
                                    crgb(140, 45, 4)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Oranges",
                                  std::array
                                  {
                                    crgb(255, 245, 235),
                                    crgb(254, 230, 206),
                                    crgb(253, 208, 162),
                                    crgb(253, 174, 107),
                                    crgb(253, 141, 60),
                                    crgb(241, 105, 19),
                                    crgb(217, 72, 1),
                                    crgb(140, 45, 4)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Oranges",
                                  std::array
                                  {
                                    crgb(255, 245, 235),
                                    crgb(254, 230, 206),
                                    crgb(253, 208, 162),
                                    crgb(253, 174, 107),
                                    crgb(253, 141, 60),
                                    crgb(241, 105, 19),
                                    crgb(217, 72, 1),
                                    crgb(166, 54, 3),
                                    crgb(127, 39, 4)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 2, 0, 0, 0, 0, 0},
                                CopyProperties {1, 2, 2, 0, 0, 0, 0},
                                ScreenProperties {1, 1, 1, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_bu_gn()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "BuGn",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BuGn",
                                  std::array
                                  {
                                    crgb(229, 245, 249),
                                    crgb(153, 216, 201),
                                    crgb(44, 162, 95)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BuGn",
                                  std::array
                                  {
                                    crgb(237, 248, 251),
                                    crgb(178, 226, 226),
                                    crgb(102, 194, 164),
                                    crgb(35, 139, 69)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BuGn",
                                  std::array
                                  {
                                    crgb(237, 248, 251),
                                    crgb(178, 226, 226),
                                    crgb(102, 194, 164),
                                    crgb(44, 162, 95),
                                    crgb(0, 109, 44)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BuGn",
                                  std::array
                                  {
                                    crgb(237, 248, 251),
                                    crgb(204, 236, 230),
                                    crgb(153, 216, 201),
                                    crgb(102, 194, 164),
                                    crgb(44, 162, 95),
                                    crgb(0, 109, 44)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BuGn",
                                  std::array
                                  {
                                    crgb(237, 248, 251),
                                    crgb(204, 236, 230),
                                    crgb(153, 216, 201),
                                    crgb(102, 194, 164),
                                    crgb(65, 174, 118),
                                    crgb(35, 139, 69),
                                    crgb(0, 88, 36)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BuGn",
                                  std::array
                                  {
                                    crgb(247, 252, 253),
                                    crgb(229, 245, 249),
                                    crgb(204, 236, 230),
                                    crgb(153, 216, 201),
                                    crgb(102, 194, 164),
                                    crgb(65, 174, 118),
                                    crgb(35, 139, 69),
                                    crgb(0, 88, 36)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "BuGn",
                                  std::array
                                  {
                                    crgb(247, 252, 253),
                                    crgb(229, 245, 249),
                                    crgb(204, 236, 230),
                                    crgb(153, 216, 201),
                                    crgb(102, 194, 164),
                                    crgb(65, 174, 118),
                                    crgb(35, 139, 69),
                                    crgb(0, 109, 44),
                                    crgb(0, 68, 27)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 2, 0, 0, 0, 0},
                                CopyProperties {1, 2, 0, 0, 0, 0, 0},
                                ScreenProperties {1, 2, 0, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_yl_or_br()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "YlOrBr",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlOrBr",
                                  std::array
                                  {
                                    crgb(255, 247, 188),
                                    crgb(254, 196, 79),
                                    crgb(217, 95, 14)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlOrBr",
                                  std::array
                                  {
                                    crgb(255, 255, 212),
                                    crgb(254, 217, 142),
                                    crgb(254, 153, 41),
                                    crgb(204, 76, 2)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlOrBr",
                                  std::array
                                  {
                                    crgb(255, 255, 212),
                                    crgb(254, 217, 142),
                                    crgb(254, 153, 41),
                                    crgb(217, 95, 14),
                                    crgb(153, 52, 4)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlOrBr",
                                  std::array
                                  {
                                    crgb(255, 255, 212),
                                    crgb(254, 227, 145),
                                    crgb(254, 196, 79),
                                    crgb(254, 153, 41),
                                    crgb(217, 95, 14),
                                    crgb(153, 52, 4)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlOrBr",
                                  std::array
                                  {
                                    crgb(255, 255, 212),
                                    crgb(254, 227, 145),
                                    crgb(254, 196, 79),
                                    crgb(254, 153, 41),
                                    crgb(236, 112, 20),
                                    crgb(204, 76, 2),
                                    crgb(140, 45, 4)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlOrBr",
                                  std::array
                                  {
                                    crgb(255, 255, 229),
                                    crgb(255, 247, 188),
                                    crgb(254, 227, 145),
                                    crgb(254, 196, 79),
                                    crgb(254, 153, 41),
                                    crgb(236, 112, 20),
                                    crgb(204, 76, 2),
                                    crgb(140, 45, 4)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlOrBr",
                                  std::array
                                  {
                                    crgb(255, 255, 229),
                                    crgb(255, 247, 188),
                                    crgb(254, 227, 145),
                                    crgb(254, 196, 79),
                                    crgb(254, 153, 41),
                                    crgb(236, 112, 20),
                                    crgb(204, 76, 2),
                                    crgb(153, 52, 4),
                                    crgb(102, 37, 6)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 2, 0, 0, 0, 0},
                                CopyProperties {1, 2, 2, 0, 0, 0, 0},
                                ScreenProperties {1, 2, 0, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_yl_gn()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "YlGn",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlGn",
                                  std::array
                                  {
                                    crgb(247, 252, 185),
                                    crgb(173, 221, 142),
                                    crgb(49, 163, 84)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlGn",
                                  std::array
                                  {
                                    crgb(255, 255, 204),
                                    crgb(194, 230, 153),
                                    crgb(120, 198, 121),
                                    crgb(35, 132, 67)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlGn",
                                  std::array
                                  {
                                    crgb(255, 255, 204),
                                    crgb(194, 230, 153),
                                    crgb(120, 198, 121),
                                    crgb(49, 163, 84),
                                    crgb(0, 104, 55)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlGn",
                                  std::array
                                  {
                                    crgb(255, 255, 204),
                                    crgb(217, 240, 163),
                                    crgb(173, 221, 142),
                                    crgb(120, 198, 121),
                                    crgb(49, 163, 84),
                                    crgb(0, 104, 55)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlGn",
                                  std::array
                                  {
                                    crgb(255, 255, 204),
                                    crgb(217, 240, 163),
                                    crgb(173, 221, 142),
                                    crgb(120, 198, 121),
                                    crgb(65, 171, 93),
                                    crgb(35, 132, 67),
                                    crgb(0, 90, 50)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlGn",
                                  std::array
                                  {
                                    crgb(255, 255, 229),
                                    crgb(247, 252, 185),
                                    crgb(217, 240, 163),
                                    crgb(173, 221, 142),
                                    crgb(120, 198, 121),
                                    crgb(65, 171, 93),
                                    crgb(35, 132, 67),
                                    crgb(0, 90, 50)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlGn",
                                  std::array
                                  {
                                    crgb(255, 255, 229),
                                    crgb(247, 252, 185),
                                    crgb(217, 240, 163),
                                    crgb(173, 221, 142),
                                    crgb(120, 198, 121),
                                    crgb(65, 171, 93),
                                    crgb(35, 132, 67),
                                    crgb(0, 104, 55),
                                    crgb(0, 69, 41)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 1, 0, 0, 0, 0},
                                CopyProperties {1, 2, 0, 0, 0, 0, 0},
                                ScreenProperties {1, 1, 1, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_reds()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "Reds",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Reds",
                                  std::array
                                  {
                                    crgb(254, 224, 210),
                                    crgb(252, 146, 114),
                                    crgb(222, 45, 38)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Reds",
                                  std::array
                                  {
                                    crgb(254, 229, 217),
                                    crgb(252, 174, 145),
                                    crgb(251, 106, 74),
                                    crgb(203, 24, 29)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Reds",
                                  std::array
                                  {
                                    crgb(254, 229, 217),
                                    crgb(252, 174, 145),
                                    crgb(251, 106, 74),
                                    crgb(222, 45, 38),
                                    crgb(165, 15, 21)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Reds",
                                  std::array
                                  {
                                    crgb(254, 229, 217),
                                    crgb(252, 187, 161),
                                    crgb(252, 146, 114),
                                    crgb(251, 106, 74),
                                    crgb(222, 45, 38),
                                    crgb(165, 15, 21)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Reds",
                                  std::array
                                  {
                                    crgb(254, 229, 217),
                                    crgb(252, 187, 161),
                                    crgb(252, 146, 114),
                                    crgb(251, 106, 74),
                                    crgb(239, 59, 44),
                                    crgb(203, 24, 29),
                                    crgb(153, 0, 13)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Reds",
                                  std::array
                                  {
                                    crgb(255, 245, 240),
                                    crgb(254, 224, 210),
                                    crgb(252, 187, 161),
                                    crgb(252, 146, 114),
                                    crgb(251, 106, 74),
                                    crgb(239, 59, 44),
                                    crgb(203, 24, 29),
                                    crgb(153, 0, 13)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Reds",
                                  std::array
                                  {
                                    crgb(255, 245, 240),
                                    crgb(254, 224, 210),
                                    crgb(252, 187, 161),
                                    crgb(252, 146, 114),
                                    crgb(251, 106, 74),
                                    crgb(239, 59, 44),
                                    crgb(203, 24, 29),
                                    crgb(165, 15, 21),
                                    crgb(103, 0, 13)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 2, 2, 0, 0, 0, 0},
                                CopyProperties {1, 2, 0, 0, 0, 0, 0},
                                ScreenProperties {1, 2, 0, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_rd_pu()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "RdPu",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdPu",
                                  std::array
                                  {
                                    crgb(253, 224, 221),
                                    crgb(250, 159, 181),
                                    crgb(197, 27, 138)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdPu",
                                  std::array
                                  {
                                    crgb(254, 235, 226),
                                    crgb(251, 180, 185),
                                    crgb(247, 104, 161),
                                    crgb(174, 1, 126)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdPu",
                                  std::array
                                  {
                                    crgb(254, 235, 226),
                                    crgb(251, 180, 185),
                                    crgb(247, 104, 161),
                                    crgb(197, 27, 138),
                                    crgb(122, 1, 119)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdPu",
                                  std::array
                                  {
                                    crgb(254, 235, 226),
                                    crgb(252, 197, 192),
                                    crgb(250, 159, 181),
                                    crgb(247, 104, 161),
                                    crgb(197, 27, 138),
                                    crgb(122, 1, 119)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdPu",
                                  std::array
                                  {
                                    crgb(254, 235, 226),
                                    crgb(252, 197, 192),
                                    crgb(250, 159, 181),
                                    crgb(247, 104, 161),
                                    crgb(221, 52, 151),
                                    crgb(174, 1, 126),
                                    crgb(122, 1, 119)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdPu",
                                  std::array
                                  {
                                    crgb(255, 247, 243),
                                    crgb(253, 224, 221),
                                    crgb(252, 197, 192),
                                    crgb(250, 159, 181),
                                    crgb(247, 104, 161),
                                    crgb(221, 52, 151),
                                    crgb(174, 1, 126),
                                    crgb(122, 1, 119)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "RdPu",
                                  std::array
                                  {
                                    crgb(255, 247, 243),
                                    crgb(253, 224, 221),
                                    crgb(252, 197, 192),
                                    crgb(250, 159, 181),
                                    crgb(247, 104, 161),
                                    crgb(221, 52, 151),
                                    crgb(174, 1, 126),
                                    crgb(122, 1, 119),
                                    crgb(73, 0, 106)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 1, 2, 0, 0, 0},
                                CopyProperties {1, 2, 0, 0, 0, 0, 0},
                                ScreenProperties {1, 1, 1, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_greens()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "Greens",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Greens",
                                  std::array
                                  {
                                    crgb(229, 245, 224),
                                    crgb(161, 217, 155),
                                    crgb(49, 163, 84)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Greens",
                                  std::array
                                  {
                                    crgb(237, 248, 233),
                                    crgb(186, 228, 179),
                                    crgb(116, 196, 118),
                                    crgb(35, 139, 69)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Greens",
                                  std::array
                                  {
                                    crgb(237, 248, 233),
                                    crgb(186, 228, 179),
                                    crgb(116, 196, 118),
                                    crgb(49, 163, 84),
                                    crgb(0, 109, 44)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Greens",
                                  std::array
                                  {
                                    crgb(237, 248, 233),
                                    crgb(199, 233, 192),
                                    crgb(161, 217, 155),
                                    crgb(116, 196, 118),
                                    crgb(49, 163, 84),
                                    crgb(0, 109, 44)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Greens",
                                  std::array
                                  {
                                    crgb(237, 248, 233),
                                    crgb(199, 233, 192),
                                    crgb(161, 217, 155),
                                    crgb(116, 196, 118),
                                    crgb(65, 171, 93),
                                    crgb(35, 139, 69),
                                    crgb(0, 90, 50)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Greens",
                                  std::array
                                  {
                                    crgb(247, 252, 245),
                                    crgb(229, 245, 224),
                                    crgb(199, 233, 192),
                                    crgb(161, 217, 155),
                                    crgb(116, 196, 118),
                                    crgb(65, 171, 93),
                                    crgb(35, 139, 69),
                                    crgb(0, 90, 50)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Greens",
                                  std::array
                                  {
                                    crgb(247, 252, 245),
                                    crgb(229, 245, 224),
                                    crgb(199, 233, 192),
                                    crgb(161, 217, 155),
                                    crgb(116, 196, 118),
                                    crgb(65, 171, 93),
                                    crgb(35, 139, 69),
                                    crgb(0, 109, 44),
                                    crgb(0, 68, 27)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 0, 0, 0, 0, 0, 0},
                                CopyProperties {1, 2, 0, 0, 0, 0, 0},
                                ScreenProperties {1, 2, 0, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_yl_gn_bu()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "YlGnBu",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlGnBu",
                                  std::array
                                  {
                                    crgb(237, 248, 177),
                                    crgb(127, 205, 187),
                                    crgb(44, 127, 184)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlGnBu",
                                  std::array
                                  {
                                    crgb(255, 255, 204),
                                    crgb(161, 218, 180),
                                    crgb(65, 182, 196),
                                    crgb(34, 94, 168)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlGnBu",
                                  std::array
                                  {
                                    crgb(255, 255, 204),
                                    crgb(161, 218, 180),
                                    crgb(65, 182, 196),
                                    crgb(44, 127, 184),
                                    crgb(37, 52, 148)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlGnBu",
                                  std::array
                                  {
                                    crgb(255, 255, 204),
                                    crgb(199, 233, 180),
                                    crgb(127, 205, 187),
                                    crgb(65, 182, 196),
                                    crgb(44, 127, 184),
                                    crgb(37, 52, 148)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlGnBu",
                                  std::array
                                  {
                                    crgb(255, 255, 204),
                                    crgb(199, 233, 180),
                                    crgb(127, 205, 187),
                                    crgb(65, 182, 196),
                                    crgb(29, 145, 192),
                                    crgb(34, 94, 168),
                                    crgb(12, 44, 132)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlGnBu",
                                  std::array
                                  {
                                    crgb(255, 255, 217),
                                    crgb(237, 248, 177),
                                    crgb(199, 233, 180),
                                    crgb(127, 205, 187),
                                    crgb(65, 182, 196),
                                    crgb(29, 145, 192),
                                    crgb(34, 94, 168),
                                    crgb(12, 44, 132)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlGnBu",
                                  std::array
                                  {
                                    crgb(255, 255, 217),
                                    crgb(237, 248, 177),
                                    crgb(199, 233, 180),
                                    crgb(127, 205, 187),
                                    crgb(65, 182, 196),
                                    crgb(29, 145, 192),
                                    crgb(34, 94, 168),
                                    crgb(37, 52, 148),
                                    crgb(8, 29, 88)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 1, 2, 2, 2, 0},
                                CopyProperties {1, 2, 0, 0, 0, 0, 0},
                                ScreenProperties {1, 1, 2, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_purples()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "Purples",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Purples",
                                  std::array
                                  {
                                    crgb(239, 237, 245),
                                    crgb(188, 189, 220),
                                    crgb(117, 107, 177)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Purples",
                                  std::array
                                  {
                                    crgb(242, 240, 247),
                                    crgb(203, 201, 226),
                                    crgb(158, 154, 200),
                                    crgb(106, 81, 163)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Purples",
                                  std::array
                                  {
                                    crgb(242, 240, 247),
                                    crgb(203, 201, 226),
                                    crgb(158, 154, 200),
                                    crgb(117, 107, 177),
                                    crgb(84, 39, 143)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Purples",
                                  std::array
                                  {
                                    crgb(242, 240, 247),
                                    crgb(218, 218, 235),
                                    crgb(188, 189, 220),
                                    crgb(158, 154, 200),
                                    crgb(117, 107, 177),
                                    crgb(84, 39, 143)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Purples",
                                  std::array
                                  {
                                    crgb(242, 240, 247),
                                    crgb(218, 218, 235),
                                    crgb(188, 189, 220),
                                    crgb(158, 154, 200),
                                    crgb(128, 125, 186),
                                    crgb(106, 81, 163),
                                    crgb(74, 20, 134)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Purples",
                                  std::array
                                  {
                                    crgb(252, 251, 253),
                                    crgb(239, 237, 245),
                                    crgb(218, 218, 235),
                                    crgb(188, 189, 220),
                                    crgb(158, 154, 200),
                                    crgb(128, 125, 186),
                                    crgb(106, 81, 163),
                                    crgb(74, 20, 134)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Purples",
                                  std::array
                                  {
                                    crgb(252, 251, 253),
                                    crgb(239, 237, 245),
                                    crgb(218, 218, 235),
                                    crgb(188, 189, 220),
                                    crgb(158, 154, 200),
                                    crgb(128, 125, 186),
                                    crgb(106, 81, 163),
                                    crgb(84, 39, 143),
                                    crgb(63, 0, 125)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 0, 0, 0, 0, 0, 0},
                                CopyProperties {1, 2, 0, 0, 0, 0, 0},
                                ScreenProperties {1, 0, 0, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_gn_bu()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "GnBu",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "GnBu",
                                  std::array
                                  {
                                    crgb(224, 243, 219),
                                    crgb(168, 221, 181),
                                    crgb(67, 162, 202)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "GnBu",
                                  std::array
                                  {
                                    crgb(240, 249, 232),
                                    crgb(186, 228, 188),
                                    crgb(123, 204, 196),
                                    crgb(43, 140, 190)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "GnBu",
                                  std::array
                                  {
                                    crgb(240, 249, 232),
                                    crgb(186, 228, 188),
                                    crgb(123, 204, 196),
                                    crgb(67, 162, 202),
                                    crgb(8, 104, 172)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "GnBu",
                                  std::array
                                  {
                                    crgb(240, 249, 232),
                                    crgb(204, 235, 197),
                                    crgb(168, 221, 181),
                                    crgb(123, 204, 196),
                                    crgb(67, 162, 202),
                                    crgb(8, 104, 172)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "GnBu",
                                  std::array
                                  {
                                    crgb(240, 249, 232),
                                    crgb(204, 235, 197),
                                    crgb(168, 221, 181),
                                    crgb(123, 204, 196),
                                    crgb(78, 179, 211),
                                    crgb(43, 140, 190),
                                    crgb(8, 88, 158)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "GnBu",
                                  std::array
                                  {
                                    crgb(247, 252, 240),
                                    crgb(224, 243, 219),
                                    crgb(204, 235, 197),
                                    crgb(168, 221, 181),
                                    crgb(123, 204, 196),
                                    crgb(78, 179, 211),
                                    crgb(43, 140, 190),
                                    crgb(8, 88, 158)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "GnBu",
                                  std::array
                                  {
                                    crgb(247, 252, 240),
                                    crgb(224, 243, 219),
                                    crgb(204, 235, 197),
                                    crgb(168, 221, 181),
                                    crgb(123, 204, 196),
                                    crgb(78, 179, 211),
                                    crgb(43, 140, 190),
                                    crgb(8, 104, 172),
                                    crgb(8, 64, 129)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 1, 2, 2, 2, 0},
                                CopyProperties {1, 2, 0, 0, 0, 0, 0},
                                ScreenProperties {1, 1, 2, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_greys()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "Greys",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Greys",
                                  std::array
                                  {
                                    crgb(240, 240, 240),
                                    crgb(189, 189, 189),
                                    crgb(99, 99, 99)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Greys",
                                  std::array
                                  {
                                    crgb(247, 247, 247),
                                    crgb(204, 204, 204),
                                    crgb(150, 150, 150),
                                    crgb(82, 82, 82)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Greys",
                                  std::array
                                  {
                                    crgb(247, 247, 247),
                                    crgb(204, 204, 204),
                                    crgb(150, 150, 150),
                                    crgb(99, 99, 99),
                                    crgb(37, 37, 37)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Greys",
                                  std::array
                                  {
                                    crgb(247, 247, 247),
                                    crgb(217, 217, 217),
                                    crgb(189, 189, 189),
                                    crgb(150, 150, 150),
                                    crgb(99, 99, 99),
                                    crgb(37, 37, 37)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Greys",
                                  std::array
                                  {
                                    crgb(247, 247, 247),
                                    crgb(217, 217, 217),
                                    crgb(189, 189, 189),
                                    crgb(150, 150, 150),
                                    crgb(115, 115, 115),
                                    crgb(82, 82, 82),
                                    crgb(37, 37, 37)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Greys",
                                  std::array
                                  {
                                    crgb(255, 255, 255),
                                    crgb(240, 240, 240),
                                    crgb(217, 217, 217),
                                    crgb(189, 189, 189),
                                    crgb(150, 150, 150),
                                    crgb(115, 115, 115),
                                    crgb(82, 82, 82),
                                    crgb(37, 37, 37)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Greys",
                                  std::array
                                  {
                                    crgb(255, 255, 255),
                                    crgb(240, 240, 240),
                                    crgb(217, 217, 217),
                                    crgb(189, 189, 189),
                                    crgb(150, 150, 150),
                                    crgb(115, 115, 115),
                                    crgb(82, 82, 82),
                                    crgb(37, 37, 37),
                                    crgb(0, 0, 0)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 2, 0, 0, 0, 0},
                                CopyProperties {1, 0, 0, 0, 0, 0, 0},
                                ScreenProperties {1, 2, 0, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_yl_or_rd()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "YlOrRd",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlOrRd",
                                  std::array
                                  {
                                    crgb(255, 237, 160),
                                    crgb(254, 178, 76),
                                    crgb(240, 59, 32)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlOrRd",
                                  std::array
                                  {
                                    crgb(255, 255, 178),
                                    crgb(254, 204, 92),
                                    crgb(253, 141, 60),
                                    crgb(227, 26, 28)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlOrRd",
                                  std::array
                                  {
                                    crgb(255, 255, 178),
                                    crgb(254, 204, 92),
                                    crgb(253, 141, 60),
                                    crgb(240, 59, 32),
                                    crgb(189, 0, 38)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlOrRd",
                                  std::array
                                  {
                                    crgb(255, 255, 178),
                                    crgb(254, 217, 118),
                                    crgb(254, 178, 76),
                                    crgb(253, 141, 60),
                                    crgb(240, 59, 32),
                                    crgb(189, 0, 38)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlOrRd",
                                  std::array
                                  {
                                    crgb(255, 255, 178),
                                    crgb(254, 217, 118),
                                    crgb(254, 178, 76),
                                    crgb(253, 141, 60),
                                    crgb(252, 78, 42),
                                    crgb(227, 26, 28),
                                    crgb(177, 0, 38)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlOrRd",
                                  std::array
                                  {
                                    crgb(255, 255, 204),
                                    crgb(255, 237, 160),
                                    crgb(254, 217, 118),
                                    crgb(254, 178, 76),
                                    crgb(253, 141, 60),
                                    crgb(252, 78, 42),
                                    crgb(227, 26, 28),
                                    crgb(177, 0, 38)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "YlOrRd",
                                  std::array
                                  {
                                    crgb(255, 255, 204),
                                    crgb(255, 237, 160),
                                    crgb(254, 217, 118),
                                    crgb(254, 178, 76),
                                    crgb(253, 141, 60),
                                    crgb(252, 78, 42),
                                    crgb(227, 26, 28),
                                    crgb(189, 0, 38),
                                    crgb(128, 0, 38)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 2, 2, 0, 0, 0},
                                CopyProperties {1, 2, 2, 0, 0, 0, 0},
                                ScreenProperties {1, 2, 2, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_pu_rd()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "PuRd",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuRd",
                                  std::array
                                  {
                                    crgb(231, 225, 239),
                                    crgb(201, 148, 199),
                                    crgb(221, 28, 119)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuRd",
                                  std::array
                                  {
                                    crgb(241, 238, 246),
                                    crgb(215, 181, 216),
                                    crgb(223, 101, 176),
                                    crgb(206, 18, 86)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuRd",
                                  std::array
                                  {
                                    crgb(241, 238, 246),
                                    crgb(215, 181, 216),
                                    crgb(223, 101, 176),
                                    crgb(221, 28, 119),
                                    crgb(152, 0, 67)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuRd",
                                  std::array
                                  {
                                    crgb(241, 238, 246),
                                    crgb(212, 185, 218),
                                    crgb(201, 148, 199),
                                    crgb(223, 101, 176),
                                    crgb(221, 28, 119),
                                    crgb(152, 0, 67)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuRd",
                                  std::array
                                  {
                                    crgb(241, 238, 246),
                                    crgb(212, 185, 218),
                                    crgb(201, 148, 199),
                                    crgb(223, 101, 176),
                                    crgb(231, 41, 138),
                                    crgb(206, 18, 86),
                                    crgb(145, 0, 63)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuRd",
                                  std::array
                                  {
                                    crgb(247, 244, 249),
                                    crgb(231, 225, 239),
                                    crgb(212, 185, 218),
                                    crgb(201, 148, 199),
                                    crgb(223, 101, 176),
                                    crgb(231, 41, 138),
                                    crgb(206, 18, 86),
                                    crgb(145, 0, 63)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuRd",
                                  std::array
                                  {
                                    crgb(247, 244, 249),
                                    crgb(231, 225, 239),
                                    crgb(212, 185, 218),
                                    crgb(201, 148, 199),
                                    crgb(223, 101, 176),
                                    crgb(231, 41, 138),
                                    crgb(206, 18, 86),
                                    crgb(152, 0, 67),
                                    crgb(103, 0, 31)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 1, 1, 0, 0, 0, 0},
                                CopyProperties {1, 2, 0, 0, 0, 0, 0},
                                ScreenProperties {1, 1, 1, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_blues()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "Blues",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Blues",
                                  std::array
                                  {
                                    crgb(222, 235, 247),
                                    crgb(158, 202, 225),
                                    crgb(49, 130, 189)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Blues",
                                  std::array
                                  {
                                    crgb(239, 243, 255),
                                    crgb(189, 215, 231),
                                    crgb(107, 174, 214),
                                    crgb(33, 113, 181)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Blues",
                                  std::array
                                  {
                                    crgb(239, 243, 255),
                                    crgb(189, 215, 231),
                                    crgb(107, 174, 214),
                                    crgb(49, 130, 189),
                                    crgb(8, 81, 156)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Blues",
                                  std::array
                                  {
                                    crgb(239, 243, 255),
                                    crgb(198, 219, 239),
                                    crgb(158, 202, 225),
                                    crgb(107, 174, 214),
                                    crgb(49, 130, 189),
                                    crgb(8, 81, 156)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Blues",
                                  std::array
                                  {
                                    crgb(239, 243, 255),
                                    crgb(198, 219, 239),
                                    crgb(158, 202, 225),
                                    crgb(107, 174, 214),
                                    crgb(66, 146, 198),
                                    crgb(33, 113, 181),
                                    crgb(8, 69, 148)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Blues",
                                  std::array
                                  {
                                    crgb(247, 251, 255),
                                    crgb(222, 235, 247),
                                    crgb(198, 219, 239),
                                    crgb(158, 202, 225),
                                    crgb(107, 174, 214),
                                    crgb(66, 146, 198),
                                    crgb(33, 113, 181),
                                    crgb(8, 69, 148)
                                  }
                              }
                          }
                         },

{
    9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "Blues",
                                  std::array
                                  {
                                    crgb(247, 251, 255),
                                    crgb(222, 235, 247),
                                    crgb(198, 219, 239),
                                    crgb(158, 202, 225),
                                    crgb(107, 174, 214),
                                    crgb(66, 146, 198),
                                    crgb(33, 113, 181),
                                    crgb(8, 81, 156),
                                    crgb(8, 48, 107)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 2, 0, 0, 0, 0, 0},
                                CopyProperties {1, 0, 0, 0, 0, 0, 0},
                                ScreenProperties {1, 2, 0, 0, 0, 0, 0}}};
            return cb;
        }

        const Colorbrewer&
        palette_sequential_pu_bu_gn()
        {
            static const Colorbrewer cb = Colorbrewer {
                    "PuBuGn",
                    SizeToPaletteMap {
                    {
                        3,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuBuGn",
                                  std::array
                                  {
                                    crgb(236, 226, 240),
                                    crgb(166, 189, 219),
                                    crgb(28, 144, 153)
                                  }
                              }
                          }
                    },

                {
                    4,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuBuGn",
                                  std::array
                                  {
                                    crgb(246, 239, 247),
                                    crgb(189, 201, 225),
                                    crgb(103, 169, 207),
                                    crgb(2, 129, 138)
                                  }
                              }
                          }
                         },

            {
                5,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuBuGn",
                                  std::array
                                  {
                                    crgb(246, 239, 247),
                                    crgb(189, 201, 225),
                                    crgb(103, 169, 207),
                                    crgb(28, 144, 153),
                                    crgb(1, 108, 89)
                                  }
                              }
                          }
                         },

        {
            6,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuBuGn",
                                  std::array
                                  {
                                    crgb(246, 239, 247),
                                    crgb(208, 209, 230),
                                    crgb(166, 189, 219),
                                    crgb(103, 169, 207),
                                    crgb(28, 144, 153),
                                    crgb(1, 108, 89)
                                  }
                              }
                          }
                         },

    {
        7,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuBuGn",
                                  std::array
                                  {
                                    crgb(246, 239, 247),
                                    crgb(208, 209, 230),
                                    crgb(166, 189, 219),
                                    crgb(103, 169, 207),
                                    crgb(54, 144, 192),
                                    crgb(2, 129, 138),
                                    crgb(1, 100, 80)
                                  }
                              }
                          }
                         },

{
    8,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuBuGn",
                                  std::array
                                  {
                                    crgb(255, 247, 251),
                                    crgb(236, 226, 240),
                                    crgb(208, 209, 230),
                                    crgb(166, 189, 219),
                                    crgb(103, 169, 207),
                                    crgb(54, 144, 192),
                                    crgb(2, 129, 138),
                                    crgb(1, 100, 80)
                                  }
                              }
                          }
                         },

                    {
                        9,
                          BrewerPalette
                          {
                              Palette
                              {
                                  "PuBuGn",
                                  std::array
                                  {
                                    crgb(255, 247, 251),
                                    crgb(236, 226, 240),
                                    crgb(208, 209, 230),
                                    crgb(166, 189, 219),
                                    crgb(103, 169, 207),
                                    crgb(54, 144, 192),
                                    crgb(2, 129, 138),
                                    crgb(1, 108, 89),
                                    crgb(1, 70, 54)
                                  }
                              }
                          }
                         }},
                    BrewerType::seq,
                    Properties {BlindProperties {1},
                                PrintProperties {1, 2, 2, 0, 0, 0, 0},
                                CopyProperties {1, 2, 0, 0, 0, 0, 0},
                                ScreenProperties {1, 1, 2, 0, 0, 0, 0}}};
            return cb;
        }


    }
}
