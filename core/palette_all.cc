#include "core/palette_all.h"

#include "core/palette.h"
#include "core/palette_cubehelix.h"
#include "core/palette_lospec.h"
#include "core/palette_matplot.h"
#include "core/palette_mycarta.h"
#include "core/palette_tableu.h"
#include "core/palette_wes.h"


namespace euphoria::core
{
    namespace palette
    {
        const Palette&
        GetPalette(PaletteName name)
        {
            switch(name)
            {
            // standard
            case PaletteName::Dawnbringer: return Dawnbringer();
            case PaletteName::NamedColors: return NamedColors();
            // cubehelix
            case PaletteName::Classic: return Classic();
            case PaletteName::Perceptual_rainbow: return Perceptual_rainbow();
            case PaletteName::Purple: return Purple();
            case PaletteName::Jim_special: return Jim_special();
            case PaletteName::Red: return Red();
            case PaletteName::Cubehelix1: return Cubehelix1();
            case PaletteName::Cubehelix2: return Cubehelix2();
            case PaletteName::Cubehelix3: return Cubehelix3();

            // lospec
            case PaletteName::OneBit: return OneBit();
            case PaletteName::TwoBitGrayScale: return TwoBitGrayScale();
            case PaletteName::ThreeBit: return ThreeBit();
            case PaletteName::ARQ4: return ARQ4();
            case PaletteName::CGA: return CGA();
            case PaletteName::EDG8: return EDG8();
            case PaletteName::EDG16: return EDG16();
            case PaletteName::EDG32: return EDG32();
            case PaletteName::EDG36: return EDG36();
            case PaletteName::EDG64: return EDG64();
            case PaletteName::EN4: return EN4();
            case PaletteName::Ink: return Ink();
            case PaletteName::Pico8: return Pico8();
            case PaletteName::Ammo8: return Ammo8();
            case PaletteName::NYX8: return NYX8();
            case PaletteName::FifteenPDX: return FifteenPDX();
            case PaletteName::TwentyPDX: return TwentyPDX();
            case PaletteName::Arne16: return Arne16();
            case PaletteName::Night16: return Night16();
            case PaletteName::AAP16: return AAP16();
            case PaletteName::AAP64: return AAP64();
            case PaletteName::Splendor128: return Splendor128();
            case PaletteName::Famicube: return Famicube();

            // matplot
            case PaletteName::Magma: return Magma();
            case PaletteName::Inferno: return Inferno();
            case PaletteName::Plasma: return Plasma();
            case PaletteName::Viridis: return Viridis();

            // mycarta
            case PaletteName::Cube1: return Cube1();
            case PaletteName::CubeYF: return CubeYF();
            case PaletteName::LinearL: return LinearL();

            // tableu
            case PaletteName::Tableau_10: return Tableau_10();
            case PaletteName::TableauLight_10: return TableauLight_10();
            case PaletteName::TableauMedium_10: return TableauMedium_10();
            case PaletteName::Tableau_20: return Tableau_20();
            case PaletteName::Gray_5: return Gray_5();
            case PaletteName::ColorBlind_10: return ColorBlind_10();
            case PaletteName::TrafficLight_9: return TrafficLight_9();
            case PaletteName::PurpleGray_6: return PurpleGray_6();
            case PaletteName::PurpleGray_12: return PurpleGray_12();
            case PaletteName::BlueRed_6: return BlueRed_6();
            case PaletteName::BlueRed_12: return BlueRed_12();
            case PaletteName::GreenOrange_6: return GreenOrange_6();
            case PaletteName::GreenOrange_12: return GreenOrange_12();

            // wes
            case PaletteName::Chevalier: return Chevalier();
            case PaletteName::Moonrise1: return Moonrise1();
            case PaletteName::Mendl: return Mendl();
            case PaletteName::Margot1: return Margot1();
            case PaletteName::Cavalcanti: return Cavalcanti();
            case PaletteName::Moonrise2: return Moonrise2();
            case PaletteName::Margot2: return Margot2();
            case PaletteName::Moonrise3: return Moonrise3();
            case PaletteName::GrandBudapest1: return GrandBudapest1();
            case PaletteName::Moonrise4: return Moonrise4();
            case PaletteName::Zissou: return Zissou();
            case PaletteName::Royal1: return Royal1();
            case PaletteName::Darjeeling1: return Darjeeling1();
            case PaletteName::FantasticFox1: return FantasticFox1();
            case PaletteName::Margot3: return Margot3();
            case PaletteName::GrandBudapest2: return GrandBudapest2();
            case PaletteName::Aquatic1: return Aquatic1();
            case PaletteName::Darjeeling2: return Darjeeling2();
            case PaletteName::FantasticFox2: return FantasticFox2();
            case PaletteName::GrandBudapest3: return GrandBudapest3();
            case PaletteName::Royal2: return Royal2();
            case PaletteName::Moonrise5: return Moonrise5();
            case PaletteName::GrandBudapest4: return GrandBudapest4();
            case PaletteName::Moonrise6: return Moonrise6();
            case PaletteName::GrandBudapest5: return GrandBudapest5();
            case PaletteName::Aquatic2: return Aquatic2();
            case PaletteName::Royal3: return Royal3();
            case PaletteName::Moonrise7: return Moonrise7();
            case PaletteName::Aquatic3: return Aquatic3();
            case PaletteName::Darjeeling3: return Darjeeling3();
            default: return OneBit();
            }
        }

    }  // namespace palette

    BEGIN_ENUM_LIST(palette::PaletteName)
    // standard
    ENUM_VALUE(palette::PaletteName, Dawnbringer)
    ENUM_VALUE(palette::PaletteName, NamedColors)

    // cubehelix
    ENUM_VALUE(palette::PaletteName, Classic)
    ENUM_VALUE(palette::PaletteName, Perceptual_rainbow)
    ENUM_VALUE(palette::PaletteName, Purple)
    ENUM_VALUE(palette::PaletteName, Jim_special)
    ENUM_VALUE(palette::PaletteName, Red)
    ENUM_VALUE(palette::PaletteName, Cubehelix1)
    ENUM_VALUE(palette::PaletteName, Cubehelix2)
    ENUM_VALUE(palette::PaletteName, Cubehelix3)

    // lospec
    ENUM_VALUE(palette::PaletteName, OneBit)
    ENUM_VALUE(palette::PaletteName, TwoBitGrayScale)
    ENUM_VALUE(palette::PaletteName, ThreeBit)
    ENUM_VALUE(palette::PaletteName, ARQ4)
    ENUM_VALUE(palette::PaletteName, CGA)
    ENUM_VALUE(palette::PaletteName, EDG8)
    ENUM_VALUE(palette::PaletteName, EDG16)
    ENUM_VALUE(palette::PaletteName, EDG32)
    ENUM_VALUE(palette::PaletteName, EDG36)
    ENUM_VALUE(palette::PaletteName, EDG64)
    ENUM_VALUE(palette::PaletteName, EN4)
    ENUM_VALUE(palette::PaletteName, Ink)
    ENUM_VALUE(palette::PaletteName, Pico8)
    ENUM_VALUE(palette::PaletteName, Ammo8)
    ENUM_VALUE(palette::PaletteName, NYX8)
    ENUM_VALUE(palette::PaletteName, FifteenPDX)
    ENUM_VALUE(palette::PaletteName, TwentyPDX)
    ENUM_VALUE(palette::PaletteName, Arne16)
    ENUM_VALUE(palette::PaletteName, Night16)
    ENUM_VALUE(palette::PaletteName, AAP16)
    ENUM_VALUE(palette::PaletteName, AAP64)
    ENUM_VALUE(palette::PaletteName, Splendor128)
    ENUM_VALUE(palette::PaletteName, Famicube)

    // matplot
    ENUM_VALUE(palette::PaletteName, Magma)
    ENUM_VALUE(palette::PaletteName, Inferno)
    ENUM_VALUE(palette::PaletteName, Plasma)
    ENUM_VALUE(palette::PaletteName, Viridis)

    // mycarta
    ENUM_VALUE(palette::PaletteName, Cube1)
    ENUM_VALUE(palette::PaletteName, CubeYF)
    ENUM_VALUE(palette::PaletteName, LinearL)

    // tableu
    ENUM_VALUE(palette::PaletteName, Tableau_10)
    ENUM_VALUE(palette::PaletteName, TableauLight_10)
    ENUM_VALUE(palette::PaletteName, TableauMedium_10)
    ENUM_VALUE(palette::PaletteName, Tableau_20)
    ENUM_VALUE(palette::PaletteName, Gray_5)
    ENUM_VALUE(palette::PaletteName, ColorBlind_10)
    ENUM_VALUE(palette::PaletteName, TrafficLight_9)
    ENUM_VALUE(palette::PaletteName, PurpleGray_6)
    ENUM_VALUE(palette::PaletteName, PurpleGray_12)
    ENUM_VALUE(palette::PaletteName, BlueRed_6)
    ENUM_VALUE(palette::PaletteName, BlueRed_12)
    ENUM_VALUE(palette::PaletteName, GreenOrange_6)
    ENUM_VALUE(palette::PaletteName, GreenOrange_12)

    // wes
    ENUM_VALUE(palette::PaletteName, Chevalier)
    ENUM_VALUE(palette::PaletteName, Moonrise1)
    ENUM_VALUE(palette::PaletteName, Mendl)
    ENUM_VALUE(palette::PaletteName, Margot1)
    ENUM_VALUE(palette::PaletteName, Cavalcanti)
    ENUM_VALUE(palette::PaletteName, Moonrise2)
    ENUM_VALUE(palette::PaletteName, Margot2)
    ENUM_VALUE(palette::PaletteName, Moonrise3)
    ENUM_VALUE(palette::PaletteName, GrandBudapest1)
    ENUM_VALUE(palette::PaletteName, Moonrise4)
    ENUM_VALUE(palette::PaletteName, Zissou)
    ENUM_VALUE(palette::PaletteName, Royal1)
    ENUM_VALUE(palette::PaletteName, Darjeeling1)
    ENUM_VALUE(palette::PaletteName, FantasticFox1)
    ENUM_VALUE(palette::PaletteName, Margot3)
    ENUM_VALUE(palette::PaletteName, GrandBudapest2)
    ENUM_VALUE(palette::PaletteName, Aquatic1)
    ENUM_VALUE(palette::PaletteName, Darjeeling2)
    ENUM_VALUE(palette::PaletteName, FantasticFox2)
    ENUM_VALUE(palette::PaletteName, GrandBudapest3)
    ENUM_VALUE(palette::PaletteName, Royal2)
    ENUM_VALUE(palette::PaletteName, Moonrise5)
    ENUM_VALUE(palette::PaletteName, GrandBudapest4)
    ENUM_VALUE(palette::PaletteName, Moonrise6)
    ENUM_VALUE(palette::PaletteName, GrandBudapest5)
    ENUM_VALUE(palette::PaletteName, Aquatic2)
    ENUM_VALUE(palette::PaletteName, Royal3)
    ENUM_VALUE(palette::PaletteName, Moonrise7)
    ENUM_VALUE(palette::PaletteName, Aquatic3)
    ENUM_VALUE(palette::PaletteName, Darjeeling3)
    END_ENUM_LIST()

}  // namespace euphoria::core
