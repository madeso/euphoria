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
        rgbi
        to_crgb(unsigned char r, unsigned char g, unsigned char b)
        {
            return rgbi {r, g, b};
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
                get_palette_diverging_spectral(), get_palette_diverging_rd_yl_gn(), get_palette_diverging_rd_bu(), get_palette_diverging_pi_y_g(), get_palette_diverging_p_r_gn(), get_palette_diverging_rd_yl_bu(),
                get_palette_diverging_br_b_g(), get_palette_diverging_rd_gy(), get_palette_diverging_pu_or(),

                /*** Qualitative ***/
                get_palette_qualitative_set2(), get_palette_qualitative_accent(), get_palette_qualitative_set1(), get_palette_qualitative_set3(), get_palette_qualitative_dark2(), get_palette_qualitative_paired(),
                get_palette_qualitative_pastel2(), get_palette_qualitative_pastel1(),

                /*** Sequential ***/
                get_palette_sequential_or_rd(), get_palette_sequential_pu_bu(), get_palette_sequential_bu_pu(), get_palette_sequential_oranges(), get_palette_sequential_bu_gn(), get_palette_sequential_yl_or_br(), get_palette_sequential_yl_gn(),
                get_palette_sequential_reds(), get_palette_sequential_rd_pu(), get_palette_sequential_greens(), get_palette_sequential_yl_gn_bu(), get_palette_sequential_purples(), get_palette_sequential_gn_bu(), get_palette_sequential_greys(),
                get_palette_sequential_yl_or_rd(), get_palette_sequential_pu_rd(), get_palette_sequential_blues(), get_palette_sequential_pu_bu_gn()
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
        get_palette_diverging_spectral()
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
                                    to_crgb(252, 141, 89),
                                    to_crgb(255, 255, 191),
                                    to_crgb(153, 213, 148)
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
                                    to_crgb(215, 25, 28),
                                    to_crgb(253, 174, 97),
                                    to_crgb(171, 221, 164),
                                    to_crgb(43, 131, 186)
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
                                    to_crgb(215, 25, 28),
                                    to_crgb(253, 174, 97),
                                    to_crgb(255, 255, 191),
                                    to_crgb(171, 221, 164),
                                    to_crgb(43, 131, 186)
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
                                    to_crgb(213, 62, 79),
                                    to_crgb(252, 141, 89),
                                    to_crgb(254, 224, 139),
                                    to_crgb(230, 245, 152),
                                    to_crgb(153, 213, 148),
                                    to_crgb(50, 136, 189)
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
                                    to_crgb(213, 62, 79),
                                    to_crgb(252, 141, 89),
                                    to_crgb(254, 224, 139),
                                    to_crgb(255, 255, 191),
                                    to_crgb(230, 245, 152),
                                    to_crgb(153, 213, 148),
                                    to_crgb(50, 136, 189)
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
                                    to_crgb(213, 62, 79),
                                    to_crgb(244, 109, 67),
                                    to_crgb(253, 174, 97),
                                    to_crgb(254, 224, 139),
                                    to_crgb(230, 245, 152),
                                    to_crgb(171, 221, 164),
                                    to_crgb(102, 194, 165),
                                    to_crgb(50, 136, 189)
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
                                    to_crgb(213, 62, 79),
                                    to_crgb(244, 109, 67),
                                    to_crgb(253, 174, 97),
                                    to_crgb(254, 224, 139),
                                    to_crgb(255, 255, 191),
                                    to_crgb(230, 245, 152),
                                    to_crgb(171, 221, 164),
                                    to_crgb(102, 194, 165),
                                    to_crgb(50, 136, 189)
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
                                    to_crgb(158, 1, 66),
                                    to_crgb(213, 62, 79),
                                    to_crgb(244, 109, 67),
                                    to_crgb(253, 174, 97),
                                    to_crgb(254, 224, 139),
                                    to_crgb(230, 245, 152),
                                    to_crgb(171, 221, 164),
                                    to_crgb(102, 194, 165),
                                    to_crgb(50, 136, 189),
                                    to_crgb(94, 79, 162)
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
                                    to_crgb(158, 1, 66),
                                    to_crgb(213, 62, 79),
                                    to_crgb(244, 109, 67),
                                    to_crgb(253, 174, 97),
                                    to_crgb(254, 224, 139),
                                    to_crgb(255, 255, 191),
                                    to_crgb(230, 245, 152),
                                    to_crgb(171, 221, 164),
                                    to_crgb(102, 194, 165),
                                    to_crgb(50, 136, 189),
                                    to_crgb(94, 79, 162)
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
        get_palette_diverging_rd_yl_gn()
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
                                    to_crgb(252, 141, 89),
                                    to_crgb(255, 255, 191),
                                    to_crgb(145, 207, 96)
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
                                    to_crgb(215, 25, 28),
                                    to_crgb(253, 174, 97),
                                    to_crgb(166, 217, 106),
                                    to_crgb(26, 150, 65)
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
                                    to_crgb(215, 25, 28),
                                    to_crgb(253, 174, 97),
                                    to_crgb(255, 255, 191),
                                    to_crgb(166, 217, 106),
                                    to_crgb(26, 150, 65)
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
                                    to_crgb(215, 48, 39),
                                    to_crgb(252, 141, 89),
                                    to_crgb(254, 224, 139),
                                    to_crgb(217, 239, 139),
                                    to_crgb(145, 207, 96),
                                    to_crgb(26, 152, 80)
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
                                    to_crgb(215, 48, 39),
                                    to_crgb(252, 141, 89),
                                    to_crgb(254, 224, 139),
                                    to_crgb(255, 255, 191),
                                    to_crgb(217, 239, 139),
                                    to_crgb(145, 207, 96),
                                    to_crgb(26, 152, 80)
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
                                    to_crgb(215, 48, 39),
                                    to_crgb(244, 109, 67),
                                    to_crgb(253, 174, 97),
                                    to_crgb(254, 224, 139),
                                    to_crgb(217, 239, 139),
                                    to_crgb(166, 217, 106),
                                    to_crgb(102, 189, 99),
                                    to_crgb(26, 152, 80)
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
                                    to_crgb(215, 48, 39),
                                    to_crgb(244, 109, 67),
                                    to_crgb(253, 174, 97),
                                    to_crgb(254, 224, 139),
                                    to_crgb(255, 255, 191),
                                    to_crgb(217, 239, 139),
                                    to_crgb(166, 217, 106),
                                    to_crgb(102, 189, 99),
                                    to_crgb(26, 152, 80)
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
                                    to_crgb(165, 0, 38),
                                    to_crgb(215, 48, 39),
                                    to_crgb(244, 109, 67),
                                    to_crgb(253, 174, 97),
                                    to_crgb(254, 224, 139),
                                    to_crgb(217, 239, 139),
                                    to_crgb(166, 217, 106),
                                    to_crgb(102, 189, 99),
                                    to_crgb(26, 152, 80),
                                    to_crgb(0, 104, 55)
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
                                    to_crgb(165, 0, 38),
                                    to_crgb(215, 48, 39),
                                    to_crgb(244, 109, 67),
                                    to_crgb(253, 174, 97),
                                    to_crgb(254, 224, 139),
                                    to_crgb(255, 255, 191),
                                    to_crgb(217, 239, 139),
                                    to_crgb(166, 217, 106),
                                    to_crgb(102, 189, 99),
                                    to_crgb(26, 152, 80),
                                    to_crgb(0, 104, 55)
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
        get_palette_diverging_rd_bu()
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
                                    to_crgb(239, 138, 98),
                                    to_crgb(247, 247, 247),
                                    to_crgb(103, 169, 207)
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
                                    to_crgb(202, 0, 32),
                                    to_crgb(244, 165, 130),
                                    to_crgb(146, 197, 222),
                                    to_crgb(5, 113, 176)
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
                                    to_crgb(202, 0, 32),
                                    to_crgb(244, 165, 130),
                                    to_crgb(247, 247, 247),
                                    to_crgb(146, 197, 222),
                                    to_crgb(5, 113, 176)
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
                                    to_crgb(178, 24, 43),
                                    to_crgb(239, 138, 98),
                                    to_crgb(253, 219, 199),
                                    to_crgb(209, 229, 240),
                                    to_crgb(103, 169, 207),
                                    to_crgb(33, 102, 172)
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
                                    to_crgb(178, 24, 43),
                                    to_crgb(239, 138, 98),
                                    to_crgb(253, 219, 199),
                                    to_crgb(247, 247, 247),
                                    to_crgb(209, 229, 240),
                                    to_crgb(103, 169, 207),
                                    to_crgb(33, 102, 172)
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
                                    to_crgb(178, 24, 43),
                                    to_crgb(214, 96, 77),
                                    to_crgb(244, 165, 130),
                                    to_crgb(253, 219, 199),
                                    to_crgb(209, 229, 240),
                                    to_crgb(146, 197, 222),
                                    to_crgb(67, 147, 195),
                                    to_crgb(33, 102, 172)
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
                                    to_crgb(178, 24, 43),
                                    to_crgb(214, 96, 77),
                                    to_crgb(244, 165, 130),
                                    to_crgb(253, 219, 199),
                                    to_crgb(247, 247, 247),
                                    to_crgb(209, 229, 240),
                                    to_crgb(146, 197, 222),
                                    to_crgb(67, 147, 195),
                                    to_crgb(33, 102, 172)
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
                                    to_crgb(103, 0, 31),
                                    to_crgb(178, 24, 43),
                                    to_crgb(214, 96, 77),
                                    to_crgb(244, 165, 130),
                                    to_crgb(253, 219, 199),
                                    to_crgb(209, 229, 240),
                                    to_crgb(146, 197, 222),
                                    to_crgb(67, 147, 195),
                                    to_crgb(33, 102, 172),
                                    to_crgb(5, 48, 97)
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
                                    to_crgb(103, 0, 31),
                                    to_crgb(178, 24, 43),
                                    to_crgb(214, 96, 77),
                                    to_crgb(244, 165, 130),
                                    to_crgb(253, 219, 199),
                                    to_crgb(247, 247, 247),
                                    to_crgb(209, 229, 240),
                                    to_crgb(146, 197, 222),
                                    to_crgb(67, 147, 195),
                                    to_crgb(33, 102, 172),
                                    to_crgb(5, 48, 97)
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
        get_palette_diverging_pi_y_g()
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
                                    to_crgb(233, 163, 201),
                                    to_crgb(247, 247, 247),
                                    to_crgb(161, 215, 106)
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
                                    to_crgb(208, 28, 139),
                                    to_crgb(241, 182, 218),
                                    to_crgb(184, 225, 134),
                                    to_crgb(77, 172, 38)
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
                                    to_crgb(208, 28, 139),
                                    to_crgb(241, 182, 218),
                                    to_crgb(247, 247, 247),
                                    to_crgb(184, 225, 134),
                                    to_crgb(77, 172, 38)
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
                                    to_crgb(197, 27, 125),
                                    to_crgb(233, 163, 201),
                                    to_crgb(253, 224, 239),
                                    to_crgb(230, 245, 208),
                                    to_crgb(161, 215, 106),
                                    to_crgb(77, 146, 33)
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
                                    to_crgb(197, 27, 125),
                                    to_crgb(233, 163, 201),
                                    to_crgb(253, 224, 239),
                                    to_crgb(247, 247, 247),
                                    to_crgb(230, 245, 208),
                                    to_crgb(161, 215, 106),
                                    to_crgb(77, 146, 33)
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
                                    to_crgb(197, 27, 125),
                                    to_crgb(222, 119, 174),
                                    to_crgb(241, 182, 218),
                                    to_crgb(253, 224, 239),
                                    to_crgb(230, 245, 208),
                                    to_crgb(184, 225, 134),
                                    to_crgb(127, 188, 65),
                                    to_crgb(77, 146, 33)
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
                                    to_crgb(197, 27, 125),
                                    to_crgb(222, 119, 174),
                                    to_crgb(241, 182, 218),
                                    to_crgb(253, 224, 239),
                                    to_crgb(247, 247, 247),
                                    to_crgb(230, 245, 208),
                                    to_crgb(184, 225, 134),
                                    to_crgb(127, 188, 65),
                                    to_crgb(77, 146, 33)
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
                                    to_crgb(142, 1, 82),
                                    to_crgb(197, 27, 125),
                                    to_crgb(222, 119, 174),
                                    to_crgb(241, 182, 218),
                                    to_crgb(253, 224, 239),
                                    to_crgb(230, 245, 208),
                                    to_crgb(184, 225, 134),
                                    to_crgb(127, 188, 65),
                                    to_crgb(77, 146, 33),
                                    to_crgb(39, 100, 25)
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
                                    to_crgb(142, 1, 82),
                                    to_crgb(197, 27, 125),
                                    to_crgb(222, 119, 174),
                                    to_crgb(241, 182, 218),
                                    to_crgb(253, 224, 239),
                                    to_crgb(247, 247, 247),
                                    to_crgb(230, 245, 208),
                                    to_crgb(184, 225, 134),
                                    to_crgb(127, 188, 65),
                                    to_crgb(77, 146, 33),
                                    to_crgb(39, 100, 25)
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
        get_palette_diverging_p_r_gn()
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
                                    to_crgb(175, 141, 195),
                                    to_crgb(247, 247, 247),
                                    to_crgb(127, 191, 123)
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
                                    to_crgb(123, 50, 148),
                                    to_crgb(194, 165, 207),
                                    to_crgb(166, 219, 160),
                                    to_crgb(0, 136, 55)
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
                                    to_crgb(123, 50, 148),
                                    to_crgb(194, 165, 207),
                                    to_crgb(247, 247, 247),
                                    to_crgb(166, 219, 160),
                                    to_crgb(0, 136, 55)
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
                                    to_crgb(118, 42, 131),
                                    to_crgb(175, 141, 195),
                                    to_crgb(231, 212, 232),
                                    to_crgb(217, 240, 211),
                                    to_crgb(127, 191, 123),
                                    to_crgb(27, 120, 55)
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
                                    to_crgb(118, 42, 131),
                                    to_crgb(175, 141, 195),
                                    to_crgb(231, 212, 232),
                                    to_crgb(247, 247, 247),
                                    to_crgb(217, 240, 211),
                                    to_crgb(127, 191, 123),
                                    to_crgb(27, 120, 55)
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
                                    to_crgb(118, 42, 131),
                                    to_crgb(153, 112, 171),
                                    to_crgb(194, 165, 207),
                                    to_crgb(231, 212, 232),
                                    to_crgb(217, 240, 211),
                                    to_crgb(166, 219, 160),
                                    to_crgb(90, 174, 97),
                                    to_crgb(27, 120, 55)
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
                                    to_crgb(118, 42, 131),
                                    to_crgb(153, 112, 171),
                                    to_crgb(194, 165, 207),
                                    to_crgb(231, 212, 232),
                                    to_crgb(247, 247, 247),
                                    to_crgb(217, 240, 211),
                                    to_crgb(166, 219, 160),
                                    to_crgb(90, 174, 97),
                                    to_crgb(27, 120, 55)
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
                                    to_crgb(64, 0, 75),
                                    to_crgb(118, 42, 131),
                                    to_crgb(153, 112, 171),
                                    to_crgb(194, 165, 207),
                                    to_crgb(231, 212, 232),
                                    to_crgb(217, 240, 211),
                                    to_crgb(166, 219, 160),
                                    to_crgb(90, 174, 97),
                                    to_crgb(27, 120, 55),
                                    to_crgb(0, 68, 27)
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
                                    to_crgb(64, 0, 75),
                                    to_crgb(118, 42, 131),
                                    to_crgb(153, 112, 171),
                                    to_crgb(194, 165, 207),
                                    to_crgb(231, 212, 232),
                                    to_crgb(247, 247, 247),
                                    to_crgb(217, 240, 211),
                                    to_crgb(166, 219, 160),
                                    to_crgb(90, 174, 97),
                                    to_crgb(27, 120, 55),
                                    to_crgb(0, 68, 27)
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
        get_palette_diverging_rd_yl_bu()
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
                                    to_crgb(252, 141, 89),
                                    to_crgb(255, 255, 191),
                                    to_crgb(145, 191, 219)
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
                                    to_crgb(215, 25, 28),
                                    to_crgb(253, 174, 97),
                                    to_crgb(171, 217, 233),
                                    to_crgb(44, 123, 182)
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
                                    to_crgb(215, 25, 28),
                                    to_crgb(253, 174, 97),
                                    to_crgb(255, 255, 191),
                                    to_crgb(171, 217, 233),
                                    to_crgb(44, 123, 182)
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
                                    to_crgb(215, 48, 39),
                                    to_crgb(252, 141, 89),
                                    to_crgb(254, 224, 144),
                                    to_crgb(224, 243, 248),
                                    to_crgb(145, 191, 219),
                                    to_crgb(69, 117, 180)
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
                                    to_crgb(215, 48, 39),
                                    to_crgb(252, 141, 89),
                                    to_crgb(254, 224, 144),
                                    to_crgb(255, 255, 191),
                                    to_crgb(224, 243, 248),
                                    to_crgb(145, 191, 219),
                                    to_crgb(69, 117, 180)
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
                                    to_crgb(215, 48, 39),
                                    to_crgb(244, 109, 67),
                                    to_crgb(253, 174, 97),
                                    to_crgb(254, 224, 144),
                                    to_crgb(224, 243, 248),
                                    to_crgb(171, 217, 233),
                                    to_crgb(116, 173, 209),
                                    to_crgb(69, 117, 180)
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
                                    to_crgb(215, 48, 39),
                                    to_crgb(244, 109, 67),
                                    to_crgb(253, 174, 97),
                                    to_crgb(254, 224, 144),
                                    to_crgb(255, 255, 191),
                                    to_crgb(224, 243, 248),
                                    to_crgb(171, 217, 233),
                                    to_crgb(116, 173, 209),
                                    to_crgb(69, 117, 180)
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
                                    to_crgb(165, 0, 38),
                                    to_crgb(215, 48, 39),
                                    to_crgb(244, 109, 67),
                                    to_crgb(253, 174, 97),
                                    to_crgb(254, 224, 144),
                                    to_crgb(224, 243, 248),
                                    to_crgb(171, 217, 233),
                                    to_crgb(116, 173, 209),
                                    to_crgb(69, 117, 180),
                                    to_crgb(49, 54, 149)
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
                                    to_crgb(165, 0, 38),
                                    to_crgb(215, 48, 39),
                                    to_crgb(244, 109, 67),
                                    to_crgb(253, 174, 97),
                                    to_crgb(254, 224, 144),
                                    to_crgb(255, 255, 191),
                                    to_crgb(224, 243, 248),
                                    to_crgb(171, 217, 233),
                                    to_crgb(116, 173, 209),
                                    to_crgb(69, 117, 180),
                                    to_crgb(49, 54, 149)
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
        get_palette_diverging_br_b_g()
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
                                    to_crgb(216, 179, 101),
                                    to_crgb(245, 245, 245),
                                    to_crgb(90, 180, 172)
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
                                    to_crgb(166, 97, 26),
                                    to_crgb(223, 194, 125),
                                    to_crgb(128, 205, 193),
                                    to_crgb(1, 133, 113)
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
                                    to_crgb(166, 97, 26),
                                    to_crgb(223, 194, 125),
                                    to_crgb(245, 245, 245),
                                    to_crgb(128, 205, 193),
                                    to_crgb(1, 133, 113)
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
                                    to_crgb(140, 81, 10),
                                    to_crgb(216, 179, 101),
                                    to_crgb(246, 232, 195),
                                    to_crgb(199, 234, 229),
                                    to_crgb(90, 180, 172),
                                    to_crgb(1, 102, 94)
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
                                    to_crgb(140, 81, 10),
                                    to_crgb(216, 179, 101),
                                    to_crgb(246, 232, 195),
                                    to_crgb(245, 245, 245),
                                    to_crgb(199, 234, 229),
                                    to_crgb(90, 180, 172),
                                    to_crgb(1, 102, 94)
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
                                    to_crgb(140, 81, 10),
                                    to_crgb(191, 129, 45),
                                    to_crgb(223, 194, 125),
                                    to_crgb(246, 232, 195),
                                    to_crgb(199, 234, 229),
                                    to_crgb(128, 205, 193),
                                    to_crgb(53, 151, 143),
                                    to_crgb(1, 102, 94)
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
                                    to_crgb(140, 81, 10),
                                    to_crgb(191, 129, 45),
                                    to_crgb(223, 194, 125),
                                    to_crgb(246, 232, 195),
                                    to_crgb(245, 245, 245),
                                    to_crgb(199, 234, 229),
                                    to_crgb(128, 205, 193),
                                    to_crgb(53, 151, 143),
                                    to_crgb(1, 102, 94)
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
                                    to_crgb(84, 48, 5),
                                    to_crgb(140, 81, 10),
                                    to_crgb(191, 129, 45),
                                    to_crgb(223, 194, 125),
                                    to_crgb(246, 232, 195),
                                    to_crgb(199, 234, 229),
                                    to_crgb(128, 205, 193),
                                    to_crgb(53, 151, 143),
                                    to_crgb(1, 102, 94),
                                    to_crgb(0, 60, 48)
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
                                    to_crgb(84, 48, 5),
                                    to_crgb(140, 81, 10),
                                    to_crgb(191, 129, 45),
                                    to_crgb(223, 194, 125),
                                    to_crgb(246, 232, 195),
                                    to_crgb(245, 245, 245),
                                    to_crgb(199, 234, 229),
                                    to_crgb(128, 205, 193),
                                    to_crgb(53, 151, 143),
                                    to_crgb(1, 102, 94),
                                    to_crgb(0, 60, 48)
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
        get_palette_diverging_rd_gy()
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
                                    to_crgb(239, 138, 98),
                                    to_crgb(255, 255, 255),
                                    to_crgb(153, 153, 153)
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
                                    to_crgb(202, 0, 32),
                                    to_crgb(244, 165, 130),
                                    to_crgb(186, 186, 186),
                                    to_crgb(64, 64, 64)
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
                                    to_crgb(202, 0, 32),
                                    to_crgb(244, 165, 130),
                                    to_crgb(255, 255, 255),
                                    to_crgb(186, 186, 186),
                                    to_crgb(64, 64, 64)
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
                                    to_crgb(178, 24, 43),
                                    to_crgb(239, 138, 98),
                                    to_crgb(253, 219, 199),
                                    to_crgb(224, 224, 224),
                                    to_crgb(153, 153, 153),
                                    to_crgb(77, 77, 77)
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
                                    to_crgb(178, 24, 43),
                                    to_crgb(239, 138, 98),
                                    to_crgb(253, 219, 199),
                                    to_crgb(255, 255, 255),
                                    to_crgb(224, 224, 224),
                                    to_crgb(153, 153, 153),
                                    to_crgb(77, 77, 77)
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
                                    to_crgb(178, 24, 43),
                                    to_crgb(214, 96, 77),
                                    to_crgb(244, 165, 130),
                                    to_crgb(253, 219, 199),
                                    to_crgb(224, 224, 224),
                                    to_crgb(186, 186, 186),
                                    to_crgb(135, 135, 135),
                                    to_crgb(77, 77, 77)
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
                                    to_crgb(178, 24, 43),
                                    to_crgb(214, 96, 77),
                                    to_crgb(244, 165, 130),
                                    to_crgb(253, 219, 199),
                                    to_crgb(255, 255, 255),
                                    to_crgb(224, 224, 224),
                                    to_crgb(186, 186, 186),
                                    to_crgb(135, 135, 135),
                                    to_crgb(77, 77, 77)
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
                                    to_crgb(103, 0, 31),
                                    to_crgb(178, 24, 43),
                                    to_crgb(214, 96, 77),
                                    to_crgb(244, 165, 130),
                                    to_crgb(253, 219, 199),
                                    to_crgb(224, 224, 224),
                                    to_crgb(186, 186, 186),
                                    to_crgb(135, 135, 135),
                                    to_crgb(77, 77, 77),
                                    to_crgb(26, 26, 26)
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
                                    to_crgb(103, 0, 31),
                                    to_crgb(178, 24, 43),
                                    to_crgb(214, 96, 77),
                                    to_crgb(244, 165, 130),
                                    to_crgb(253, 219, 199),
                                    to_crgb(255, 255, 255),
                                    to_crgb(224, 224, 224),
                                    to_crgb(186, 186, 186),
                                    to_crgb(135, 135, 135),
                                    to_crgb(77, 77, 77),
                                    to_crgb(26, 26, 26)
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
        get_palette_diverging_pu_or()
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
                                    to_crgb(241, 163, 64),
                                    to_crgb(247, 247, 247),
                                    to_crgb(153, 142, 195)
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
                                    to_crgb(230, 97, 1),
                                    to_crgb(253, 184, 99),
                                    to_crgb(178, 171, 210),
                                    to_crgb(94, 60, 153)
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
                                    to_crgb(230, 97, 1),
                                    to_crgb(253, 184, 99),
                                    to_crgb(247, 247, 247),
                                    to_crgb(178, 171, 210),
                                    to_crgb(94, 60, 153)
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
                                    to_crgb(179, 88, 6),
                                    to_crgb(241, 163, 64),
                                    to_crgb(254, 224, 182),
                                    to_crgb(216, 218, 235),
                                    to_crgb(153, 142, 195),
                                    to_crgb(84, 39, 136)
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
                                    to_crgb(179, 88, 6),
                                    to_crgb(241, 163, 64),
                                    to_crgb(254, 224, 182),
                                    to_crgb(247, 247, 247),
                                    to_crgb(216, 218, 235),
                                    to_crgb(153, 142, 195),
                                    to_crgb(84, 39, 136)
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
                                    to_crgb(179, 88, 6),
                                    to_crgb(224, 130, 20),
                                    to_crgb(253, 184, 99),
                                    to_crgb(254, 224, 182),
                                    to_crgb(216, 218, 235),
                                    to_crgb(178, 171, 210),
                                    to_crgb(128, 115, 172),
                                    to_crgb(84, 39, 136)
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
                                    to_crgb(179, 88, 6),
                                    to_crgb(224, 130, 20),
                                    to_crgb(253, 184, 99),
                                    to_crgb(254, 224, 182),
                                    to_crgb(247, 247, 247),
                                    to_crgb(216, 218, 235),
                                    to_crgb(178, 171, 210),
                                    to_crgb(128, 115, 172),
                                    to_crgb(84, 39, 136)
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
                                    to_crgb(127, 59, 8),
                                    to_crgb(179, 88, 6),
                                    to_crgb(224, 130, 20),
                                    to_crgb(253, 184, 99),
                                    to_crgb(254, 224, 182),
                                    to_crgb(216, 218, 235),
                                    to_crgb(178, 171, 210),
                                    to_crgb(128, 115, 172),
                                    to_crgb(84, 39, 136),
                                    to_crgb(45, 0, 75)
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
                                    to_crgb(127, 59, 8),
                                    to_crgb(179, 88, 6),
                                    to_crgb(224, 130, 20),
                                    to_crgb(253, 184, 99),
                                    to_crgb(254, 224, 182),
                                    to_crgb(247, 247, 247),
                                    to_crgb(216, 218, 235),
                                    to_crgb(178, 171, 210),
                                    to_crgb(128, 115, 172),
                                    to_crgb(84, 39, 136),
                                    to_crgb(45, 0, 75)
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
        get_palette_qualitative_set2()
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
                                        to_crgb(102, 194, 165),
                                        to_crgb(252, 141, 98),
                                        to_crgb(141, 160, 203)
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
                                        to_crgb(102, 194, 165),
                                        to_crgb(252, 141, 98),
                                        to_crgb(141, 160, 203),
                                        to_crgb(231, 138, 195)
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
                                        to_crgb(102, 194, 165),
                                        to_crgb(252, 141, 98),
                                        to_crgb(141, 160, 203),
                                        to_crgb(231, 138, 195),
                                        to_crgb(166, 216, 84)
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
                                        to_crgb(102, 194, 165),
                                        to_crgb(252, 141, 98),
                                        to_crgb(141, 160, 203),
                                        to_crgb(231, 138, 195),
                                        to_crgb(166, 216, 84),
                                        to_crgb(255, 217, 47)
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
                                        to_crgb(102, 194, 165),
                                        to_crgb(252, 141, 98),
                                        to_crgb(141, 160, 203),
                                        to_crgb(231, 138, 195),
                                        to_crgb(166, 216, 84),
                                        to_crgb(255, 217, 47),
                                        to_crgb(229, 196, 148)
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
                                        to_crgb(102, 194, 165),
                                        to_crgb(252, 141, 98),
                                        to_crgb(141, 160, 203),
                                        to_crgb(231, 138, 195),
                                        to_crgb(166, 216, 84),
                                        to_crgb(255, 217, 47),
                                        to_crgb(229, 196, 148),
                                        to_crgb(179, 179, 179)
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
        get_palette_qualitative_accent()
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
                                    to_crgb(127, 201, 127),
                                    to_crgb(190, 174, 212),
                                    to_crgb(253, 192, 134)
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
                                    to_crgb(127, 201, 127),
                                    to_crgb(190, 174, 212),
                                    to_crgb(253, 192, 134),
                                    to_crgb(255, 255, 153)
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
                                    to_crgb(127, 201, 127),
                                    to_crgb(190, 174, 212),
                                    to_crgb(253, 192, 134),
                                    to_crgb(255, 255, 153),
                                    to_crgb(56, 108, 176)
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
                                    to_crgb(127, 201, 127),
                                    to_crgb(190, 174, 212),
                                    to_crgb(253, 192, 134),
                                    to_crgb(255, 255, 153),
                                    to_crgb(56, 108, 176),
                                    to_crgb(240, 2, 127)
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
                                    to_crgb(127, 201, 127),
                                    to_crgb(190, 174, 212),
                                    to_crgb(253, 192, 134),
                                    to_crgb(255, 255, 153),
                                    to_crgb(56, 108, 176),
                                    to_crgb(240, 2, 127),
                                    to_crgb(191, 91, 23)
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
                                    to_crgb(127, 201, 127),
                                    to_crgb(190, 174, 212),
                                    to_crgb(253, 192, 134),
                                    to_crgb(255, 255, 153),
                                    to_crgb(56, 108, 176),
                                    to_crgb(240, 2, 127),
                                    to_crgb(191, 91, 23),
                                    to_crgb(102, 102, 102)
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
        get_palette_qualitative_set1()
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
                                    to_crgb(228, 26, 28),
                                    to_crgb(55, 126, 184),
                                    to_crgb(77, 175, 74)
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
                                    to_crgb(228, 26, 28),
                                    to_crgb(55, 126, 184),
                                    to_crgb(77, 175, 74),
                                    to_crgb(152, 78, 163)
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
                                    to_crgb(228, 26, 28),
                                    to_crgb(55, 126, 184),
                                    to_crgb(77, 175, 74),
                                    to_crgb(152, 78, 163),
                                    to_crgb(255, 127, 0)
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
                                    to_crgb(228, 26, 28),
                                    to_crgb(55, 126, 184),
                                    to_crgb(77, 175, 74),
                                    to_crgb(152, 78, 163),
                                    to_crgb(255, 127, 0),
                                    to_crgb(255, 255, 51)
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
                                    to_crgb(228, 26, 28),
                                    to_crgb(55, 126, 184),
                                    to_crgb(77, 175, 74),
                                    to_crgb(152, 78, 163),
                                    to_crgb(255, 127, 0),
                                    to_crgb(255, 255, 51),
                                    to_crgb(166, 86, 40)
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
                                    to_crgb(228, 26, 28),
                                    to_crgb(55, 126, 184),
                                    to_crgb(77, 175, 74),
                                    to_crgb(152, 78, 163),
                                    to_crgb(255, 127, 0),
                                    to_crgb(255, 255, 51),
                                    to_crgb(166, 86, 40),
                                    to_crgb(247, 129, 191)
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
                                    to_crgb(228, 26, 28),
                                    to_crgb(55, 126, 184),
                                    to_crgb(77, 175, 74),
                                    to_crgb(152, 78, 163),
                                    to_crgb(255, 127, 0),
                                    to_crgb(255, 255, 51),
                                    to_crgb(166, 86, 40),
                                    to_crgb(247, 129, 191),
                                    to_crgb(153, 153, 153)
                                  }
                              }
                          }
                         }},
                    BrewerType::qual,
                    Properties {BlindProperties {2}, PrintProperties {1}, CopyProperties {0}, ScreenProperties {1}}};
            return cb;
        }

        const Colorbrewer&
        get_palette_qualitative_set3()
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
                                    to_crgb(141, 211, 199),
                                    to_crgb(255, 255, 179),
                                    to_crgb(190, 186, 218)
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
                                    to_crgb(141, 211, 199),
                                    to_crgb(255, 255, 179),
                                    to_crgb(190, 186, 218),
                                    to_crgb(251, 128, 114)
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
                                    to_crgb(141, 211, 199),
                                    to_crgb(255, 255, 179),
                                    to_crgb(190, 186, 218),
                                    to_crgb(251, 128, 114),
                                    to_crgb(128, 177, 211)
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
                                    to_crgb(141, 211, 199),
                                    to_crgb(255, 255, 179),
                                    to_crgb(190, 186, 218),
                                    to_crgb(251, 128, 114),
                                    to_crgb(128, 177, 211),
                                    to_crgb(253, 180, 98)
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
                                    to_crgb(141, 211, 199),
                                    to_crgb(255, 255, 179),
                                    to_crgb(190, 186, 218),
                                    to_crgb(251, 128, 114),
                                    to_crgb(128, 177, 211),
                                    to_crgb(253, 180, 98),
                                    to_crgb(179, 222, 105)
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
                                    to_crgb(141, 211, 199),
                                    to_crgb(255, 255, 179),
                                    to_crgb(190, 186, 218),
                                    to_crgb(251, 128, 114),
                                    to_crgb(128, 177, 211),
                                    to_crgb(253, 180, 98),
                                    to_crgb(179, 222, 105),
                                    to_crgb(252, 205, 229)
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
                                    to_crgb(141, 211, 199),
                                    to_crgb(255, 255, 179),
                                    to_crgb(190, 186, 218),
                                    to_crgb(251, 128, 114),
                                    to_crgb(128, 177, 211),
                                    to_crgb(253, 180, 98),
                                    to_crgb(179, 222, 105),
                                    to_crgb(252, 205, 229),
                                    to_crgb(217, 217, 217)
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
                                    to_crgb(141, 211, 199),
                                    to_crgb(255, 255, 179),
                                    to_crgb(190, 186, 218),
                                    to_crgb(251, 128, 114),
                                    to_crgb(128, 177, 211),
                                    to_crgb(253, 180, 98),
                                    to_crgb(179, 222, 105),
                                    to_crgb(252, 205, 229),
                                    to_crgb(217, 217, 217),
                                    to_crgb(188, 128, 189)
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
                                    to_crgb(141, 211, 199),
                                    to_crgb(255, 255, 179),
                                    to_crgb(190, 186, 218),
                                    to_crgb(251, 128, 114),
                                    to_crgb(128, 177, 211),
                                    to_crgb(253, 180, 98),
                                    to_crgb(179, 222, 105),
                                    to_crgb(252, 205, 229),
                                    to_crgb(217, 217, 217),
                                    to_crgb(188, 128, 189),
                                    to_crgb(204, 235, 197)
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
                                    to_crgb(141, 211, 199),
                                    to_crgb(255, 255, 179),
                                    to_crgb(190, 186, 218),
                                    to_crgb(251, 128, 114),
                                    to_crgb(128, 177, 211),
                                    to_crgb(253, 180, 98),
                                    to_crgb(179, 222, 105),
                                    to_crgb(252, 205, 229),
                                    to_crgb(217, 217, 217),
                                    to_crgb(188, 128, 189),
                                    to_crgb(204, 235, 197),
                                    to_crgb(255, 237, 111)
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
        get_palette_qualitative_dark2()
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
                                    to_crgb(27, 158, 119),
                                    to_crgb(217, 95, 2),
                                    to_crgb(117, 112, 179)
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
                                    to_crgb(27, 158, 119),
                                    to_crgb(217, 95, 2),
                                    to_crgb(117, 112, 179),
                                    to_crgb(231, 41, 138)
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
                                    to_crgb(27, 158, 119),
                                    to_crgb(217, 95, 2),
                                    to_crgb(117, 112, 179),
                                    to_crgb(231, 41, 138),
                                    to_crgb(102, 166, 30)
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
                                    to_crgb(27, 158, 119),
                                    to_crgb(217, 95, 2),
                                    to_crgb(117, 112, 179),
                                    to_crgb(231, 41, 138),
                                    to_crgb(102, 166, 30),
                                    to_crgb(230, 171, 2)
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
                                    to_crgb(27, 158, 119),
                                    to_crgb(217, 95, 2),
                                    to_crgb(117, 112, 179),
                                    to_crgb(231, 41, 138),
                                    to_crgb(102, 166, 30),
                                    to_crgb(230, 171, 2),
                                    to_crgb(166, 118, 29)
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
                                    to_crgb(27, 158, 119),
                                    to_crgb(217, 95, 2),
                                    to_crgb(117, 112, 179),
                                    to_crgb(231, 41, 138),
                                    to_crgb(102, 166, 30),
                                    to_crgb(230, 171, 2),
                                    to_crgb(166, 118, 29),
                                    to_crgb(102, 102, 102)
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
        get_palette_qualitative_paired()
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
                                        to_crgb(166, 206, 227),
                                        to_crgb(31, 120, 180),
                                        to_crgb(178, 223, 138)
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
                                        to_crgb(166, 206, 227),
                                        to_crgb(31, 120, 180),
                                        to_crgb(178, 223, 138),
                                        to_crgb(51, 160, 44)
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
                                        to_crgb(166, 206, 227),
                                        to_crgb(31, 120, 180),
                                        to_crgb(178, 223, 138),
                                        to_crgb(51, 160, 44),
                                        to_crgb(251, 154, 153)
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
                                        to_crgb(166, 206, 227),
                                        to_crgb(31, 120, 180),
                                        to_crgb(178, 223, 138),
                                        to_crgb(51, 160, 44),
                                        to_crgb(251, 154, 153),
                                        to_crgb(227, 26, 28)
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
                                        to_crgb(166, 206, 227),
                                        to_crgb(31, 120, 180),
                                        to_crgb(178, 223, 138),
                                        to_crgb(51, 160, 44),
                                        to_crgb(251, 154, 153),
                                        to_crgb(227, 26, 28),
                                        to_crgb(253, 191, 111)
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
                                        to_crgb(166, 206, 227),
                                        to_crgb(31, 120, 180),
                                        to_crgb(178, 223, 138),
                                        to_crgb(51, 160, 44),
                                        to_crgb(251, 154, 153),
                                        to_crgb(227, 26, 28),
                                        to_crgb(253, 191, 111),
                                        to_crgb(255, 127, 0)
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
                                        to_crgb(166, 206, 227),
                                        to_crgb(31, 120, 180),
                                        to_crgb(178, 223, 138),
                                        to_crgb(51, 160, 44),
                                        to_crgb(251, 154, 153),
                                        to_crgb(227, 26, 28),
                                        to_crgb(253, 191, 111),
                                        to_crgb(255, 127, 0),
                                        to_crgb(202, 178, 214)
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
                                        to_crgb(166, 206, 227),
                                        to_crgb(31, 120, 180),
                                        to_crgb(178, 223, 138),
                                        to_crgb(51, 160, 44),
                                        to_crgb(251, 154, 153),
                                        to_crgb(227, 26, 28),
                                        to_crgb(253, 191, 111),
                                        to_crgb(255, 127, 0),
                                        to_crgb(202, 178, 214),
                                        to_crgb(106, 61, 154)
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
                                        to_crgb(166, 206, 227),
                                        to_crgb(31, 120, 180),
                                        to_crgb(178, 223, 138),
                                        to_crgb(51, 160, 44),
                                        to_crgb(251, 154, 153),
                                        to_crgb(227, 26, 28),
                                        to_crgb(253, 191, 111),
                                        to_crgb(255, 127, 0),
                                        to_crgb(202, 178, 214),
                                        to_crgb(106, 61, 154),
                                        to_crgb(255, 255, 153)
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
                                        to_crgb(166, 206, 227),
                                        to_crgb(31, 120, 180),
                                        to_crgb(178, 223, 138),
                                        to_crgb(51, 160, 44),
                                        to_crgb(251, 154, 153),
                                        to_crgb(227, 26, 28),
                                        to_crgb(253, 191, 111),
                                        to_crgb(255, 127, 0),
                                        to_crgb(202, 178, 214),
                                        to_crgb(106, 61, 154),
                                        to_crgb(255, 255, 153),
                                        to_crgb(177, 89, 40)
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
        get_palette_qualitative_pastel2()
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
                                    to_crgb(179, 226, 205),
                                    to_crgb(253, 205, 172),
                                    to_crgb(203, 213, 232)
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
                                    to_crgb(179, 226, 205),
                                    to_crgb(253, 205, 172),
                                    to_crgb(203, 213, 232),
                                    to_crgb(244, 202, 228)
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
                                    to_crgb(179, 226, 205),
                                    to_crgb(253, 205, 172),
                                    to_crgb(203, 213, 232),
                                    to_crgb(244, 202, 228),
                                    to_crgb(230, 245, 201)
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
                                    to_crgb(179, 226, 205),
                                    to_crgb(253, 205, 172),
                                    to_crgb(203, 213, 232),
                                    to_crgb(244, 202, 228),
                                    to_crgb(230, 245, 201),
                                    to_crgb(255, 242, 174)
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
                                    to_crgb(179, 226, 205),
                                    to_crgb(253, 205, 172),
                                    to_crgb(203, 213, 232),
                                    to_crgb(244, 202, 228),
                                    to_crgb(230, 245, 201),
                                    to_crgb(255, 242, 174),
                                    to_crgb(241, 226, 204)
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
                                    to_crgb(179, 226, 205),
                                    to_crgb(253, 205, 172),
                                    to_crgb(203, 213, 232),
                                    to_crgb(244, 202, 228),
                                    to_crgb(230, 245, 201),
                                    to_crgb(255, 242, 174),
                                    to_crgb(241, 226, 204),
                                    to_crgb(204, 204, 204)
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
        get_palette_qualitative_pastel1()
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
                                    to_crgb(251, 180, 174),
                                    to_crgb(179, 205, 227),
                                    to_crgb(204, 235, 197)
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
                                    to_crgb(251, 180, 174),
                                    to_crgb(179, 205, 227),
                                    to_crgb(204, 235, 197),
                                    to_crgb(222, 203, 228)
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
                                    to_crgb(251, 180, 174),
                                    to_crgb(179, 205, 227),
                                    to_crgb(204, 235, 197),
                                    to_crgb(222, 203, 228),
                                    to_crgb(254, 217, 166)
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
                                    to_crgb(251, 180, 174),
                                    to_crgb(179, 205, 227),
                                    to_crgb(204, 235, 197),
                                    to_crgb(222, 203, 228),
                                    to_crgb(254, 217, 166),
                                    to_crgb(255, 255, 204)
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
                                    to_crgb(251, 180, 174),
                                    to_crgb(179, 205, 227),
                                    to_crgb(204, 235, 197),
                                    to_crgb(222, 203, 228),
                                    to_crgb(254, 217, 166),
                                    to_crgb(255, 255, 204),
                                    to_crgb(229, 216, 189)
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
                                    to_crgb(251, 180, 174),
                                    to_crgb(179, 205, 227),
                                    to_crgb(204, 235, 197),
                                    to_crgb(222, 203, 228),
                                    to_crgb(254, 217, 166),
                                    to_crgb(255, 255, 204),
                                    to_crgb(229, 216, 189),
                                    to_crgb(253, 218, 236)
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
                                    to_crgb(251, 180, 174),
                                    to_crgb(179, 205, 227),
                                    to_crgb(204, 235, 197),
                                    to_crgb(222, 203, 228),
                                    to_crgb(254, 217, 166),
                                    to_crgb(255, 255, 204),
                                    to_crgb(229, 216, 189),
                                    to_crgb(253, 218, 236),
                                    to_crgb(242, 242, 242)
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
        get_palette_sequential_or_rd()
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
                                    to_crgb(254, 232, 200),
                                    to_crgb(253, 187, 132),
                                    to_crgb(227, 74, 51)
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
                                    to_crgb(254, 240, 217),
                                    to_crgb(253, 204, 138),
                                    to_crgb(252, 141, 89),
                                    to_crgb(215, 48, 31)
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
                                    to_crgb(254, 240, 217),
                                    to_crgb(253, 204, 138),
                                    to_crgb(252, 141, 89),
                                    to_crgb(227, 74, 51),
                                    to_crgb(179, 0, 0)
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
                                    to_crgb(254, 240, 217),
                                    to_crgb(253, 212, 158),
                                    to_crgb(253, 187, 132),
                                    to_crgb(252, 141, 89),
                                    to_crgb(227, 74, 51),
                                    to_crgb(179, 0, 0)
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
                                    to_crgb(254, 240, 217),
                                    to_crgb(253, 212, 158),
                                    to_crgb(253, 187, 132),
                                    to_crgb(252, 141, 89),
                                    to_crgb(239, 101, 72),
                                    to_crgb(215, 48, 31),
                                    to_crgb(153, 0, 0)
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
                                    to_crgb(255, 247, 236),
                                    to_crgb(254, 232, 200),
                                    to_crgb(253, 212, 158),
                                    to_crgb(253, 187, 132),
                                    to_crgb(252, 141, 89),
                                    to_crgb(239, 101, 72),
                                    to_crgb(215, 48, 31),
                                    to_crgb(153, 0, 0)
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
                                    to_crgb(255, 247, 236),
                                    to_crgb(254, 232, 200),
                                    to_crgb(253, 212, 158),
                                    to_crgb(253, 187, 132),
                                    to_crgb(252, 141, 89),
                                    to_crgb(239, 101, 72),
                                    to_crgb(215, 48, 31),
                                    to_crgb(179, 0, 0),
                                    to_crgb(127, 0, 0)
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
        get_palette_sequential_pu_bu()
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
                                    to_crgb(236, 231, 242),
                                    to_crgb(166, 189, 219),
                                    to_crgb(43, 140, 190)
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
                                    to_crgb(241, 238, 246),
                                    to_crgb(189, 201, 225),
                                    to_crgb(116, 169, 207),
                                    to_crgb(5, 112, 176)
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
                                    to_crgb(241, 238, 246),
                                    to_crgb(189, 201, 225),
                                    to_crgb(116, 169, 207),
                                    to_crgb(43, 140, 190),
                                    to_crgb(4, 90, 141)
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
                                    to_crgb(241, 238, 246),
                                    to_crgb(208, 209, 230),
                                    to_crgb(166, 189, 219),
                                    to_crgb(116, 169, 207),
                                    to_crgb(43, 140, 190),
                                    to_crgb(4, 90, 141)
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
                                    to_crgb(241, 238, 246),
                                    to_crgb(208, 209, 230),
                                    to_crgb(166, 189, 219),
                                    to_crgb(116, 169, 207),
                                    to_crgb(54, 144, 192),
                                    to_crgb(5, 112, 176),
                                    to_crgb(3, 78, 123)
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
                                    to_crgb(255, 247, 251),
                                    to_crgb(236, 231, 242),
                                    to_crgb(208, 209, 230),
                                    to_crgb(166, 189, 219),
                                    to_crgb(116, 169, 207),
                                    to_crgb(54, 144, 192),
                                    to_crgb(5, 112, 176),
                                    to_crgb(3, 78, 123)
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
                                    to_crgb(255, 247, 251),
                                    to_crgb(236, 231, 242),
                                    to_crgb(208, 209, 230),
                                    to_crgb(166, 189, 219),
                                    to_crgb(116, 169, 207),
                                    to_crgb(54, 144, 192),
                                    to_crgb(5, 112, 176),
                                    to_crgb(4, 90, 141),
                                    to_crgb(2, 56, 88)
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
        get_palette_sequential_bu_pu()
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
                                    to_crgb(224, 236, 244),
                                    to_crgb(158, 188, 218),
                                    to_crgb(136, 86, 167)
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
                                    to_crgb(237, 248, 251),
                                    to_crgb(179, 205, 227),
                                    to_crgb(140, 150, 198),
                                    to_crgb(136, 65, 157)
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
                                    to_crgb(237, 248, 251),
                                    to_crgb(179, 205, 227),
                                    to_crgb(140, 150, 198),
                                    to_crgb(136, 86, 167),
                                    to_crgb(129, 15, 124)
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
                                    to_crgb(237, 248, 251),
                                    to_crgb(191, 211, 230),
                                    to_crgb(158, 188, 218),
                                    to_crgb(140, 150, 198),
                                    to_crgb(136, 86, 167),
                                    to_crgb(129, 15, 124)
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
                                    to_crgb(237, 248, 251),
                                    to_crgb(191, 211, 230),
                                    to_crgb(158, 188, 218),
                                    to_crgb(140, 150, 198),
                                    to_crgb(140, 107, 177),
                                    to_crgb(136, 65, 157),
                                    to_crgb(110, 1, 107)
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
                                    to_crgb(247, 252, 253),
                                    to_crgb(224, 236, 244),
                                    to_crgb(191, 211, 230),
                                    to_crgb(158, 188, 218),
                                    to_crgb(140, 150, 198),
                                    to_crgb(140, 107, 177),
                                    to_crgb(136, 65, 157),
                                    to_crgb(110, 1, 107)
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
                                    to_crgb(247, 252, 253),
                                    to_crgb(224, 236, 244),
                                    to_crgb(191, 211, 230),
                                    to_crgb(158, 188, 218),
                                    to_crgb(140, 150, 198),
                                    to_crgb(140, 107, 177),
                                    to_crgb(136, 65, 157),
                                    to_crgb(129, 15, 124),
                                    to_crgb(77, 0, 75)
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
        get_palette_sequential_oranges()
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
                                    to_crgb(254, 230, 206),
                                    to_crgb(253, 174, 107),
                                    to_crgb(230, 85, 13)
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
                                    to_crgb(254, 237, 222),
                                    to_crgb(253, 190, 133),
                                    to_crgb(253, 141, 60),
                                    to_crgb(217, 71, 1)
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
                                    to_crgb(254, 237, 222),
                                    to_crgb(253, 190, 133),
                                    to_crgb(253, 141, 60),
                                    to_crgb(230, 85, 13),
                                    to_crgb(166, 54, 3)
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
                                    to_crgb(254, 237, 222),
                                    to_crgb(253, 208, 162),
                                    to_crgb(253, 174, 107),
                                    to_crgb(253, 141, 60),
                                    to_crgb(230, 85, 13),
                                    to_crgb(166, 54, 3)
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
                                    to_crgb(254, 237, 222),
                                    to_crgb(253, 208, 162),
                                    to_crgb(253, 174, 107),
                                    to_crgb(253, 141, 60),
                                    to_crgb(241, 105, 19),
                                    to_crgb(217, 72, 1),
                                    to_crgb(140, 45, 4)
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
                                    to_crgb(255, 245, 235),
                                    to_crgb(254, 230, 206),
                                    to_crgb(253, 208, 162),
                                    to_crgb(253, 174, 107),
                                    to_crgb(253, 141, 60),
                                    to_crgb(241, 105, 19),
                                    to_crgb(217, 72, 1),
                                    to_crgb(140, 45, 4)
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
                                    to_crgb(255, 245, 235),
                                    to_crgb(254, 230, 206),
                                    to_crgb(253, 208, 162),
                                    to_crgb(253, 174, 107),
                                    to_crgb(253, 141, 60),
                                    to_crgb(241, 105, 19),
                                    to_crgb(217, 72, 1),
                                    to_crgb(166, 54, 3),
                                    to_crgb(127, 39, 4)
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
        get_palette_sequential_bu_gn()
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
                                    to_crgb(229, 245, 249),
                                    to_crgb(153, 216, 201),
                                    to_crgb(44, 162, 95)
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
                                    to_crgb(237, 248, 251),
                                    to_crgb(178, 226, 226),
                                    to_crgb(102, 194, 164),
                                    to_crgb(35, 139, 69)
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
                                    to_crgb(237, 248, 251),
                                    to_crgb(178, 226, 226),
                                    to_crgb(102, 194, 164),
                                    to_crgb(44, 162, 95),
                                    to_crgb(0, 109, 44)
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
                                    to_crgb(237, 248, 251),
                                    to_crgb(204, 236, 230),
                                    to_crgb(153, 216, 201),
                                    to_crgb(102, 194, 164),
                                    to_crgb(44, 162, 95),
                                    to_crgb(0, 109, 44)
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
                                    to_crgb(237, 248, 251),
                                    to_crgb(204, 236, 230),
                                    to_crgb(153, 216, 201),
                                    to_crgb(102, 194, 164),
                                    to_crgb(65, 174, 118),
                                    to_crgb(35, 139, 69),
                                    to_crgb(0, 88, 36)
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
                                    to_crgb(247, 252, 253),
                                    to_crgb(229, 245, 249),
                                    to_crgb(204, 236, 230),
                                    to_crgb(153, 216, 201),
                                    to_crgb(102, 194, 164),
                                    to_crgb(65, 174, 118),
                                    to_crgb(35, 139, 69),
                                    to_crgb(0, 88, 36)
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
                                    to_crgb(247, 252, 253),
                                    to_crgb(229, 245, 249),
                                    to_crgb(204, 236, 230),
                                    to_crgb(153, 216, 201),
                                    to_crgb(102, 194, 164),
                                    to_crgb(65, 174, 118),
                                    to_crgb(35, 139, 69),
                                    to_crgb(0, 109, 44),
                                    to_crgb(0, 68, 27)
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
        get_palette_sequential_yl_or_br()
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
                                    to_crgb(255, 247, 188),
                                    to_crgb(254, 196, 79),
                                    to_crgb(217, 95, 14)
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
                                    to_crgb(255, 255, 212),
                                    to_crgb(254, 217, 142),
                                    to_crgb(254, 153, 41),
                                    to_crgb(204, 76, 2)
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
                                    to_crgb(255, 255, 212),
                                    to_crgb(254, 217, 142),
                                    to_crgb(254, 153, 41),
                                    to_crgb(217, 95, 14),
                                    to_crgb(153, 52, 4)
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
                                    to_crgb(255, 255, 212),
                                    to_crgb(254, 227, 145),
                                    to_crgb(254, 196, 79),
                                    to_crgb(254, 153, 41),
                                    to_crgb(217, 95, 14),
                                    to_crgb(153, 52, 4)
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
                                    to_crgb(255, 255, 212),
                                    to_crgb(254, 227, 145),
                                    to_crgb(254, 196, 79),
                                    to_crgb(254, 153, 41),
                                    to_crgb(236, 112, 20),
                                    to_crgb(204, 76, 2),
                                    to_crgb(140, 45, 4)
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
                                    to_crgb(255, 255, 229),
                                    to_crgb(255, 247, 188),
                                    to_crgb(254, 227, 145),
                                    to_crgb(254, 196, 79),
                                    to_crgb(254, 153, 41),
                                    to_crgb(236, 112, 20),
                                    to_crgb(204, 76, 2),
                                    to_crgb(140, 45, 4)
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
                                    to_crgb(255, 255, 229),
                                    to_crgb(255, 247, 188),
                                    to_crgb(254, 227, 145),
                                    to_crgb(254, 196, 79),
                                    to_crgb(254, 153, 41),
                                    to_crgb(236, 112, 20),
                                    to_crgb(204, 76, 2),
                                    to_crgb(153, 52, 4),
                                    to_crgb(102, 37, 6)
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
        get_palette_sequential_yl_gn()
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
                                    to_crgb(247, 252, 185),
                                    to_crgb(173, 221, 142),
                                    to_crgb(49, 163, 84)
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
                                    to_crgb(255, 255, 204),
                                    to_crgb(194, 230, 153),
                                    to_crgb(120, 198, 121),
                                    to_crgb(35, 132, 67)
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
                                    to_crgb(255, 255, 204),
                                    to_crgb(194, 230, 153),
                                    to_crgb(120, 198, 121),
                                    to_crgb(49, 163, 84),
                                    to_crgb(0, 104, 55)
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
                                    to_crgb(255, 255, 204),
                                    to_crgb(217, 240, 163),
                                    to_crgb(173, 221, 142),
                                    to_crgb(120, 198, 121),
                                    to_crgb(49, 163, 84),
                                    to_crgb(0, 104, 55)
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
                                    to_crgb(255, 255, 204),
                                    to_crgb(217, 240, 163),
                                    to_crgb(173, 221, 142),
                                    to_crgb(120, 198, 121),
                                    to_crgb(65, 171, 93),
                                    to_crgb(35, 132, 67),
                                    to_crgb(0, 90, 50)
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
                                    to_crgb(255, 255, 229),
                                    to_crgb(247, 252, 185),
                                    to_crgb(217, 240, 163),
                                    to_crgb(173, 221, 142),
                                    to_crgb(120, 198, 121),
                                    to_crgb(65, 171, 93),
                                    to_crgb(35, 132, 67),
                                    to_crgb(0, 90, 50)
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
                                    to_crgb(255, 255, 229),
                                    to_crgb(247, 252, 185),
                                    to_crgb(217, 240, 163),
                                    to_crgb(173, 221, 142),
                                    to_crgb(120, 198, 121),
                                    to_crgb(65, 171, 93),
                                    to_crgb(35, 132, 67),
                                    to_crgb(0, 104, 55),
                                    to_crgb(0, 69, 41)
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
        get_palette_sequential_reds()
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
                                    to_crgb(254, 224, 210),
                                    to_crgb(252, 146, 114),
                                    to_crgb(222, 45, 38)
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
                                    to_crgb(254, 229, 217),
                                    to_crgb(252, 174, 145),
                                    to_crgb(251, 106, 74),
                                    to_crgb(203, 24, 29)
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
                                    to_crgb(254, 229, 217),
                                    to_crgb(252, 174, 145),
                                    to_crgb(251, 106, 74),
                                    to_crgb(222, 45, 38),
                                    to_crgb(165, 15, 21)
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
                                    to_crgb(254, 229, 217),
                                    to_crgb(252, 187, 161),
                                    to_crgb(252, 146, 114),
                                    to_crgb(251, 106, 74),
                                    to_crgb(222, 45, 38),
                                    to_crgb(165, 15, 21)
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
                                    to_crgb(254, 229, 217),
                                    to_crgb(252, 187, 161),
                                    to_crgb(252, 146, 114),
                                    to_crgb(251, 106, 74),
                                    to_crgb(239, 59, 44),
                                    to_crgb(203, 24, 29),
                                    to_crgb(153, 0, 13)
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
                                    to_crgb(255, 245, 240),
                                    to_crgb(254, 224, 210),
                                    to_crgb(252, 187, 161),
                                    to_crgb(252, 146, 114),
                                    to_crgb(251, 106, 74),
                                    to_crgb(239, 59, 44),
                                    to_crgb(203, 24, 29),
                                    to_crgb(153, 0, 13)
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
                                    to_crgb(255, 245, 240),
                                    to_crgb(254, 224, 210),
                                    to_crgb(252, 187, 161),
                                    to_crgb(252, 146, 114),
                                    to_crgb(251, 106, 74),
                                    to_crgb(239, 59, 44),
                                    to_crgb(203, 24, 29),
                                    to_crgb(165, 15, 21),
                                    to_crgb(103, 0, 13)
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
        get_palette_sequential_rd_pu()
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
                                    to_crgb(253, 224, 221),
                                    to_crgb(250, 159, 181),
                                    to_crgb(197, 27, 138)
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
                                    to_crgb(254, 235, 226),
                                    to_crgb(251, 180, 185),
                                    to_crgb(247, 104, 161),
                                    to_crgb(174, 1, 126)
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
                                    to_crgb(254, 235, 226),
                                    to_crgb(251, 180, 185),
                                    to_crgb(247, 104, 161),
                                    to_crgb(197, 27, 138),
                                    to_crgb(122, 1, 119)
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
                                    to_crgb(254, 235, 226),
                                    to_crgb(252, 197, 192),
                                    to_crgb(250, 159, 181),
                                    to_crgb(247, 104, 161),
                                    to_crgb(197, 27, 138),
                                    to_crgb(122, 1, 119)
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
                                    to_crgb(254, 235, 226),
                                    to_crgb(252, 197, 192),
                                    to_crgb(250, 159, 181),
                                    to_crgb(247, 104, 161),
                                    to_crgb(221, 52, 151),
                                    to_crgb(174, 1, 126),
                                    to_crgb(122, 1, 119)
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
                                    to_crgb(255, 247, 243),
                                    to_crgb(253, 224, 221),
                                    to_crgb(252, 197, 192),
                                    to_crgb(250, 159, 181),
                                    to_crgb(247, 104, 161),
                                    to_crgb(221, 52, 151),
                                    to_crgb(174, 1, 126),
                                    to_crgb(122, 1, 119)
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
                                    to_crgb(255, 247, 243),
                                    to_crgb(253, 224, 221),
                                    to_crgb(252, 197, 192),
                                    to_crgb(250, 159, 181),
                                    to_crgb(247, 104, 161),
                                    to_crgb(221, 52, 151),
                                    to_crgb(174, 1, 126),
                                    to_crgb(122, 1, 119),
                                    to_crgb(73, 0, 106)
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
        get_palette_sequential_greens()
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
                                    to_crgb(229, 245, 224),
                                    to_crgb(161, 217, 155),
                                    to_crgb(49, 163, 84)
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
                                    to_crgb(237, 248, 233),
                                    to_crgb(186, 228, 179),
                                    to_crgb(116, 196, 118),
                                    to_crgb(35, 139, 69)
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
                                    to_crgb(237, 248, 233),
                                    to_crgb(186, 228, 179),
                                    to_crgb(116, 196, 118),
                                    to_crgb(49, 163, 84),
                                    to_crgb(0, 109, 44)
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
                                    to_crgb(237, 248, 233),
                                    to_crgb(199, 233, 192),
                                    to_crgb(161, 217, 155),
                                    to_crgb(116, 196, 118),
                                    to_crgb(49, 163, 84),
                                    to_crgb(0, 109, 44)
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
                                    to_crgb(237, 248, 233),
                                    to_crgb(199, 233, 192),
                                    to_crgb(161, 217, 155),
                                    to_crgb(116, 196, 118),
                                    to_crgb(65, 171, 93),
                                    to_crgb(35, 139, 69),
                                    to_crgb(0, 90, 50)
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
                                    to_crgb(247, 252, 245),
                                    to_crgb(229, 245, 224),
                                    to_crgb(199, 233, 192),
                                    to_crgb(161, 217, 155),
                                    to_crgb(116, 196, 118),
                                    to_crgb(65, 171, 93),
                                    to_crgb(35, 139, 69),
                                    to_crgb(0, 90, 50)
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
                                    to_crgb(247, 252, 245),
                                    to_crgb(229, 245, 224),
                                    to_crgb(199, 233, 192),
                                    to_crgb(161, 217, 155),
                                    to_crgb(116, 196, 118),
                                    to_crgb(65, 171, 93),
                                    to_crgb(35, 139, 69),
                                    to_crgb(0, 109, 44),
                                    to_crgb(0, 68, 27)
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
        get_palette_sequential_yl_gn_bu()
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
                                    to_crgb(237, 248, 177),
                                    to_crgb(127, 205, 187),
                                    to_crgb(44, 127, 184)
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
                                    to_crgb(255, 255, 204),
                                    to_crgb(161, 218, 180),
                                    to_crgb(65, 182, 196),
                                    to_crgb(34, 94, 168)
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
                                    to_crgb(255, 255, 204),
                                    to_crgb(161, 218, 180),
                                    to_crgb(65, 182, 196),
                                    to_crgb(44, 127, 184),
                                    to_crgb(37, 52, 148)
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
                                    to_crgb(255, 255, 204),
                                    to_crgb(199, 233, 180),
                                    to_crgb(127, 205, 187),
                                    to_crgb(65, 182, 196),
                                    to_crgb(44, 127, 184),
                                    to_crgb(37, 52, 148)
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
                                    to_crgb(255, 255, 204),
                                    to_crgb(199, 233, 180),
                                    to_crgb(127, 205, 187),
                                    to_crgb(65, 182, 196),
                                    to_crgb(29, 145, 192),
                                    to_crgb(34, 94, 168),
                                    to_crgb(12, 44, 132)
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
                                    to_crgb(255, 255, 217),
                                    to_crgb(237, 248, 177),
                                    to_crgb(199, 233, 180),
                                    to_crgb(127, 205, 187),
                                    to_crgb(65, 182, 196),
                                    to_crgb(29, 145, 192),
                                    to_crgb(34, 94, 168),
                                    to_crgb(12, 44, 132)
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
                                    to_crgb(255, 255, 217),
                                    to_crgb(237, 248, 177),
                                    to_crgb(199, 233, 180),
                                    to_crgb(127, 205, 187),
                                    to_crgb(65, 182, 196),
                                    to_crgb(29, 145, 192),
                                    to_crgb(34, 94, 168),
                                    to_crgb(37, 52, 148),
                                    to_crgb(8, 29, 88)
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
        get_palette_sequential_purples()
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
                                    to_crgb(239, 237, 245),
                                    to_crgb(188, 189, 220),
                                    to_crgb(117, 107, 177)
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
                                    to_crgb(242, 240, 247),
                                    to_crgb(203, 201, 226),
                                    to_crgb(158, 154, 200),
                                    to_crgb(106, 81, 163)
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
                                    to_crgb(242, 240, 247),
                                    to_crgb(203, 201, 226),
                                    to_crgb(158, 154, 200),
                                    to_crgb(117, 107, 177),
                                    to_crgb(84, 39, 143)
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
                                    to_crgb(242, 240, 247),
                                    to_crgb(218, 218, 235),
                                    to_crgb(188, 189, 220),
                                    to_crgb(158, 154, 200),
                                    to_crgb(117, 107, 177),
                                    to_crgb(84, 39, 143)
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
                                    to_crgb(242, 240, 247),
                                    to_crgb(218, 218, 235),
                                    to_crgb(188, 189, 220),
                                    to_crgb(158, 154, 200),
                                    to_crgb(128, 125, 186),
                                    to_crgb(106, 81, 163),
                                    to_crgb(74, 20, 134)
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
                                    to_crgb(252, 251, 253),
                                    to_crgb(239, 237, 245),
                                    to_crgb(218, 218, 235),
                                    to_crgb(188, 189, 220),
                                    to_crgb(158, 154, 200),
                                    to_crgb(128, 125, 186),
                                    to_crgb(106, 81, 163),
                                    to_crgb(74, 20, 134)
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
                                    to_crgb(252, 251, 253),
                                    to_crgb(239, 237, 245),
                                    to_crgb(218, 218, 235),
                                    to_crgb(188, 189, 220),
                                    to_crgb(158, 154, 200),
                                    to_crgb(128, 125, 186),
                                    to_crgb(106, 81, 163),
                                    to_crgb(84, 39, 143),
                                    to_crgb(63, 0, 125)
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
        get_palette_sequential_gn_bu()
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
                                    to_crgb(224, 243, 219),
                                    to_crgb(168, 221, 181),
                                    to_crgb(67, 162, 202)
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
                                    to_crgb(240, 249, 232),
                                    to_crgb(186, 228, 188),
                                    to_crgb(123, 204, 196),
                                    to_crgb(43, 140, 190)
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
                                    to_crgb(240, 249, 232),
                                    to_crgb(186, 228, 188),
                                    to_crgb(123, 204, 196),
                                    to_crgb(67, 162, 202),
                                    to_crgb(8, 104, 172)
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
                                    to_crgb(240, 249, 232),
                                    to_crgb(204, 235, 197),
                                    to_crgb(168, 221, 181),
                                    to_crgb(123, 204, 196),
                                    to_crgb(67, 162, 202),
                                    to_crgb(8, 104, 172)
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
                                    to_crgb(240, 249, 232),
                                    to_crgb(204, 235, 197),
                                    to_crgb(168, 221, 181),
                                    to_crgb(123, 204, 196),
                                    to_crgb(78, 179, 211),
                                    to_crgb(43, 140, 190),
                                    to_crgb(8, 88, 158)
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
                                    to_crgb(247, 252, 240),
                                    to_crgb(224, 243, 219),
                                    to_crgb(204, 235, 197),
                                    to_crgb(168, 221, 181),
                                    to_crgb(123, 204, 196),
                                    to_crgb(78, 179, 211),
                                    to_crgb(43, 140, 190),
                                    to_crgb(8, 88, 158)
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
                                    to_crgb(247, 252, 240),
                                    to_crgb(224, 243, 219),
                                    to_crgb(204, 235, 197),
                                    to_crgb(168, 221, 181),
                                    to_crgb(123, 204, 196),
                                    to_crgb(78, 179, 211),
                                    to_crgb(43, 140, 190),
                                    to_crgb(8, 104, 172),
                                    to_crgb(8, 64, 129)
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
        get_palette_sequential_greys()
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
                                    to_crgb(240, 240, 240),
                                    to_crgb(189, 189, 189),
                                    to_crgb(99, 99, 99)
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
                                    to_crgb(247, 247, 247),
                                    to_crgb(204, 204, 204),
                                    to_crgb(150, 150, 150),
                                    to_crgb(82, 82, 82)
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
                                    to_crgb(247, 247, 247),
                                    to_crgb(204, 204, 204),
                                    to_crgb(150, 150, 150),
                                    to_crgb(99, 99, 99),
                                    to_crgb(37, 37, 37)
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
                                    to_crgb(247, 247, 247),
                                    to_crgb(217, 217, 217),
                                    to_crgb(189, 189, 189),
                                    to_crgb(150, 150, 150),
                                    to_crgb(99, 99, 99),
                                    to_crgb(37, 37, 37)
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
                                    to_crgb(247, 247, 247),
                                    to_crgb(217, 217, 217),
                                    to_crgb(189, 189, 189),
                                    to_crgb(150, 150, 150),
                                    to_crgb(115, 115, 115),
                                    to_crgb(82, 82, 82),
                                    to_crgb(37, 37, 37)
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
                                    to_crgb(255, 255, 255),
                                    to_crgb(240, 240, 240),
                                    to_crgb(217, 217, 217),
                                    to_crgb(189, 189, 189),
                                    to_crgb(150, 150, 150),
                                    to_crgb(115, 115, 115),
                                    to_crgb(82, 82, 82),
                                    to_crgb(37, 37, 37)
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
                                    to_crgb(255, 255, 255),
                                    to_crgb(240, 240, 240),
                                    to_crgb(217, 217, 217),
                                    to_crgb(189, 189, 189),
                                    to_crgb(150, 150, 150),
                                    to_crgb(115, 115, 115),
                                    to_crgb(82, 82, 82),
                                    to_crgb(37, 37, 37),
                                    to_crgb(0, 0, 0)
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
        get_palette_sequential_yl_or_rd()
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
                                    to_crgb(255, 237, 160),
                                    to_crgb(254, 178, 76),
                                    to_crgb(240, 59, 32)
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
                                    to_crgb(255, 255, 178),
                                    to_crgb(254, 204, 92),
                                    to_crgb(253, 141, 60),
                                    to_crgb(227, 26, 28)
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
                                    to_crgb(255, 255, 178),
                                    to_crgb(254, 204, 92),
                                    to_crgb(253, 141, 60),
                                    to_crgb(240, 59, 32),
                                    to_crgb(189, 0, 38)
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
                                    to_crgb(255, 255, 178),
                                    to_crgb(254, 217, 118),
                                    to_crgb(254, 178, 76),
                                    to_crgb(253, 141, 60),
                                    to_crgb(240, 59, 32),
                                    to_crgb(189, 0, 38)
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
                                    to_crgb(255, 255, 178),
                                    to_crgb(254, 217, 118),
                                    to_crgb(254, 178, 76),
                                    to_crgb(253, 141, 60),
                                    to_crgb(252, 78, 42),
                                    to_crgb(227, 26, 28),
                                    to_crgb(177, 0, 38)
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
                                    to_crgb(255, 255, 204),
                                    to_crgb(255, 237, 160),
                                    to_crgb(254, 217, 118),
                                    to_crgb(254, 178, 76),
                                    to_crgb(253, 141, 60),
                                    to_crgb(252, 78, 42),
                                    to_crgb(227, 26, 28),
                                    to_crgb(177, 0, 38)
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
                                    to_crgb(255, 255, 204),
                                    to_crgb(255, 237, 160),
                                    to_crgb(254, 217, 118),
                                    to_crgb(254, 178, 76),
                                    to_crgb(253, 141, 60),
                                    to_crgb(252, 78, 42),
                                    to_crgb(227, 26, 28),
                                    to_crgb(189, 0, 38),
                                    to_crgb(128, 0, 38)
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
        get_palette_sequential_pu_rd()
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
                                    to_crgb(231, 225, 239),
                                    to_crgb(201, 148, 199),
                                    to_crgb(221, 28, 119)
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
                                    to_crgb(241, 238, 246),
                                    to_crgb(215, 181, 216),
                                    to_crgb(223, 101, 176),
                                    to_crgb(206, 18, 86)
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
                                    to_crgb(241, 238, 246),
                                    to_crgb(215, 181, 216),
                                    to_crgb(223, 101, 176),
                                    to_crgb(221, 28, 119),
                                    to_crgb(152, 0, 67)
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
                                    to_crgb(241, 238, 246),
                                    to_crgb(212, 185, 218),
                                    to_crgb(201, 148, 199),
                                    to_crgb(223, 101, 176),
                                    to_crgb(221, 28, 119),
                                    to_crgb(152, 0, 67)
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
                                    to_crgb(241, 238, 246),
                                    to_crgb(212, 185, 218),
                                    to_crgb(201, 148, 199),
                                    to_crgb(223, 101, 176),
                                    to_crgb(231, 41, 138),
                                    to_crgb(206, 18, 86),
                                    to_crgb(145, 0, 63)
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
                                    to_crgb(247, 244, 249),
                                    to_crgb(231, 225, 239),
                                    to_crgb(212, 185, 218),
                                    to_crgb(201, 148, 199),
                                    to_crgb(223, 101, 176),
                                    to_crgb(231, 41, 138),
                                    to_crgb(206, 18, 86),
                                    to_crgb(145, 0, 63)
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
                                    to_crgb(247, 244, 249),
                                    to_crgb(231, 225, 239),
                                    to_crgb(212, 185, 218),
                                    to_crgb(201, 148, 199),
                                    to_crgb(223, 101, 176),
                                    to_crgb(231, 41, 138),
                                    to_crgb(206, 18, 86),
                                    to_crgb(152, 0, 67),
                                    to_crgb(103, 0, 31)
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
        get_palette_sequential_blues()
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
                                    to_crgb(222, 235, 247),
                                    to_crgb(158, 202, 225),
                                    to_crgb(49, 130, 189)
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
                                    to_crgb(239, 243, 255),
                                    to_crgb(189, 215, 231),
                                    to_crgb(107, 174, 214),
                                    to_crgb(33, 113, 181)
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
                                    to_crgb(239, 243, 255),
                                    to_crgb(189, 215, 231),
                                    to_crgb(107, 174, 214),
                                    to_crgb(49, 130, 189),
                                    to_crgb(8, 81, 156)
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
                                    to_crgb(239, 243, 255),
                                    to_crgb(198, 219, 239),
                                    to_crgb(158, 202, 225),
                                    to_crgb(107, 174, 214),
                                    to_crgb(49, 130, 189),
                                    to_crgb(8, 81, 156)
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
                                    to_crgb(239, 243, 255),
                                    to_crgb(198, 219, 239),
                                    to_crgb(158, 202, 225),
                                    to_crgb(107, 174, 214),
                                    to_crgb(66, 146, 198),
                                    to_crgb(33, 113, 181),
                                    to_crgb(8, 69, 148)
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
                                    to_crgb(247, 251, 255),
                                    to_crgb(222, 235, 247),
                                    to_crgb(198, 219, 239),
                                    to_crgb(158, 202, 225),
                                    to_crgb(107, 174, 214),
                                    to_crgb(66, 146, 198),
                                    to_crgb(33, 113, 181),
                                    to_crgb(8, 69, 148)
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
                                    to_crgb(247, 251, 255),
                                    to_crgb(222, 235, 247),
                                    to_crgb(198, 219, 239),
                                    to_crgb(158, 202, 225),
                                    to_crgb(107, 174, 214),
                                    to_crgb(66, 146, 198),
                                    to_crgb(33, 113, 181),
                                    to_crgb(8, 81, 156),
                                    to_crgb(8, 48, 107)
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
        get_palette_sequential_pu_bu_gn()
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
                                    to_crgb(236, 226, 240),
                                    to_crgb(166, 189, 219),
                                    to_crgb(28, 144, 153)
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
                                    to_crgb(246, 239, 247),
                                    to_crgb(189, 201, 225),
                                    to_crgb(103, 169, 207),
                                    to_crgb(2, 129, 138)
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
                                    to_crgb(246, 239, 247),
                                    to_crgb(189, 201, 225),
                                    to_crgb(103, 169, 207),
                                    to_crgb(28, 144, 153),
                                    to_crgb(1, 108, 89)
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
                                    to_crgb(246, 239, 247),
                                    to_crgb(208, 209, 230),
                                    to_crgb(166, 189, 219),
                                    to_crgb(103, 169, 207),
                                    to_crgb(28, 144, 153),
                                    to_crgb(1, 108, 89)
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
                                    to_crgb(246, 239, 247),
                                    to_crgb(208, 209, 230),
                                    to_crgb(166, 189, 219),
                                    to_crgb(103, 169, 207),
                                    to_crgb(54, 144, 192),
                                    to_crgb(2, 129, 138),
                                    to_crgb(1, 100, 80)
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
                                    to_crgb(255, 247, 251),
                                    to_crgb(236, 226, 240),
                                    to_crgb(208, 209, 230),
                                    to_crgb(166, 189, 219),
                                    to_crgb(103, 169, 207),
                                    to_crgb(54, 144, 192),
                                    to_crgb(2, 129, 138),
                                    to_crgb(1, 100, 80)
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
                                    to_crgb(255, 247, 251),
                                    to_crgb(236, 226, 240),
                                    to_crgb(208, 209, 230),
                                    to_crgb(166, 189, 219),
                                    to_crgb(103, 169, 207),
                                    to_crgb(54, 144, 192),
                                    to_crgb(2, 129, 138),
                                    to_crgb(1, 108, 89),
                                    to_crgb(1, 70, 54)
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
