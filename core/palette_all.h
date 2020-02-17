#ifndef EUPHORIA_PALETTE_ALL_H
#define EUPHORIA_PALETTE_ALL_H

#include <string>
#include <array>

#include "core/enumtostring.h"

namespace euphoria::core
{
    struct Palette;
}

namespace euphoria::core::palette
{
    enum class PaletteName
    {
        // standard
        Dawnbringer,
        NamedColors,

        // cubehelix
        Classic,
        Perceptual_rainbow,
        Purple,
        Jim_special,
        Red,
        Cubehelix1,
        Cubehelix2,
        Cubehelix3,

        // lospec
        OneBit,
        TwoBitGrayScale,
        ThreeBit,
        ARQ4,
        CGA,
        EDG8,
        EDG16,
        EDG32,
        EDG36,
        EDG64,
        EN4,
        Ink,
        Pico8,
        Ammo8,
        NYX8,
        FifteenPDX,
        TwentyPDX,
        Arne16,
        Night16,
        AAP16,
        AAP64,
        Splendor128,
        Famicube,

        // matplot
        Magma,
        Inferno,
        Plasma,
        Viridis,

        // mycarta
        Cube1,
        CubeYF,
        LinearL,

        // tableu
        Tableau_10,
        TableauLight_10,
        TableauMedium_10,
        Tableau_20,
        Gray_5,
        ColorBlind_10,
        TrafficLight_9,
        PurpleGray_6,
        PurpleGray_12,
        BlueRed_6,
        BlueRed_12,
        GreenOrange_6,
        GreenOrange_12,


        // wes
        Chevalier,
        Moonrise1,
        Mendl,
        Margot1,
        Cavalcanti,
        Moonrise2,
        Margot2,
        Moonrise3,
        GrandBudapest1,
        Moonrise4,
        Zissou,
        Royal1,
        Darjeeling1,
        FantasticFox1,
        Margot3,
        GrandBudapest2,
        Aquatic1,
        Darjeeling2,
        FantasticFox2,
        GrandBudapest3,
        Royal2,
        Moonrise5,
        GrandBudapest4,
        Moonrise6,
        GrandBudapest5,
        Aquatic2,
        Royal3,
        Moonrise7,
        Aquatic3,
        Darjeeling3
    };

    const Palette&
    GetPalette(PaletteName name);

    constexpr std::array PaletteNames
    {
        // standard
        PaletteName::Dawnbringer,
        PaletteName::NamedColors,

        // cubehelix
        PaletteName::Classic,
        PaletteName::Perceptual_rainbow,
        PaletteName::Purple,
        PaletteName::Jim_special,
        PaletteName::Red,
        PaletteName::Cubehelix1,
        PaletteName::Cubehelix2,
        PaletteName::Cubehelix3,

        // lospec
        PaletteName::OneBit,
        PaletteName::TwoBitGrayScale,
        PaletteName::ThreeBit,
        PaletteName::ARQ4,
        PaletteName::CGA,
        PaletteName::EDG8,
        PaletteName::EDG16,
        PaletteName::EDG32,
        PaletteName::EDG36,
        PaletteName::EDG64,
        PaletteName::EN4,
        PaletteName::Ink,
        PaletteName::Pico8,
        PaletteName::Ammo8,
        PaletteName::NYX8,
        PaletteName::FifteenPDX,
        PaletteName::TwentyPDX,
        PaletteName::Arne16,
        PaletteName::Night16,
        PaletteName::AAP16,
        PaletteName::AAP64,
        PaletteName::Splendor128,
        PaletteName::Famicube,

        // matplot
        PaletteName::Magma,
        PaletteName::Inferno,
        PaletteName::Plasma,
        PaletteName::Viridis,

        // mycarta
        PaletteName::Cube1,
        PaletteName::CubeYF,
        PaletteName::LinearL,

        // tableu
        PaletteName::Tableau_10,
        PaletteName::TableauLight_10,
        PaletteName::TableauMedium_10,
        PaletteName::Tableau_20,
        PaletteName::Gray_5,
        PaletteName::ColorBlind_10,
        PaletteName::TrafficLight_9,
        PaletteName::PurpleGray_6,
        PaletteName::PurpleGray_12,
        PaletteName::BlueRed_6,
        PaletteName::BlueRed_12,
        PaletteName::GreenOrange_6,
        PaletteName::GreenOrange_12,


        // wes
        PaletteName::Chevalier,
        PaletteName::Moonrise1,
        PaletteName::Mendl,
        PaletteName::Margot1,
        PaletteName::Cavalcanti,
        PaletteName::Moonrise2,
        PaletteName::Margot2,
        PaletteName::Moonrise3,
        PaletteName::GrandBudapest1,
        PaletteName::Moonrise4,
        PaletteName::Zissou,
        PaletteName::Royal1,
        PaletteName::Darjeeling1,
        PaletteName::FantasticFox1,
        PaletteName::Margot3,
        PaletteName::GrandBudapest2,
        PaletteName::Aquatic1,
        PaletteName::Darjeeling2,
        PaletteName::FantasticFox2,
        PaletteName::GrandBudapest3,
        PaletteName::Royal2,
        PaletteName::Moonrise5,
        PaletteName::GrandBudapest4,
        PaletteName::Moonrise6,
        PaletteName::GrandBudapest5,
        PaletteName::Aquatic2,
        PaletteName::Royal3,
        PaletteName::Moonrise7,
        PaletteName::Aquatic3,
        PaletteName::Darjeeling3
    };
}  // namespace euphoria::core::palette

DECLARE_ENUM_LIST(euphoria::core::palette::PaletteName)


#endif  // EUPHORIA_PALETTE_ALL_H
