#include "core/palette.h"

#include "assert/assert.h"
#include "core/numeric.h"
#include "core/random.h"
#include "core/colors.h"


namespace euphoria::core
{
    namespace
    {
        float
        get_distance_squared(const Rgbi& lhs, const Rgbi& rhs)
        {
            return
                square(static_cast<float>(lhs.r - rhs.r)) +
                square(static_cast<float>(lhs.g - rhs.g)) +
                square(static_cast<float>(lhs.b - rhs.b))
                ;
        }
    }

    const Rgbi&
    Palette::get_random_color(Random* r) const
    {
        return get_random_item_in_vector(r, colors);
    }


    const Rgbi&
    Palette::get_safe_index(unsigned int i) const
    {
        return colors[i % colors.size()];
    }


    const Rgbi&
    Palette::get_closest_color(const Rgbi& c) const
    {
        return colors[get_index_closest(c)];
    }


    int
    Palette::get_index_closest(const Rgbi& c) const
    {
        ASSERT(!colors.empty());
        auto diff_best = get_distance_squared(c, colors[0]);

        int index_best = 0;
        const int size =  c_sizet_to_int(colors.size());
        for(int index = 1; index < size; index += 1)
        {
            const auto diff = get_distance_squared(c, colors[index]);
            if(diff < diff_best)
            {
                diff_best = diff;
                index_best = index;
            }
        }

        return index_best;
    }

    ///////////////////////////////////////////////////////////////////////////

    Palette
    DynamicPalette::to_palette() const
    {
        return {name, colors};
    }


    DynamicPalette::DynamicPalette(const std::string& n, const std::vector<Rgbi>& c)
        : name(n)
        , colors(c)
    {
    }


    DynamicPalette
    DynamicPalette::create_empty(const std::string& name)
    {
        return DynamicPalette{name};
    }


    DynamicPalette
    DynamicPalette::create_rainbow(int count, float saturation, float lightness)
    {
        return create_rainbow
        (
            count,
            Angle::from_radians(0),
            Angle::from_percent_of_360
            (
                max(1.0f, static_cast<float>(count - 1) / static_cast<float>(count))
            ),
            saturation,
            lightness
        );
    }


    DynamicPalette
    DynamicPalette::create_rainbow
    (
        int count,
        const Angle& from,
        const Angle& to,
        float saturation,
        float lightness
    )
    {
        ASSERT(count > 1);

        auto pal = DynamicPalette::create_empty("Rainbow");

        for(int i = 0; i < count; i += 1)
        {
            float d = static_cast<float>(i) / static_cast<float>(count - 1);
            const auto rgbf = crgb
            (
                Hsl
                {
                    AngleTransform::transform(from, d, to),
                    saturation,
                    lightness
                }
            );
            pal.colors.push_back(crgbi(rgbf));
        }

        return pal;
    }


    DynamicPalette::DynamicPalette(const std::string& n)
        : name(n)
    {
    }


    ////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        Rgbi
        hex(unsigned int hex)
        {
            return Rgbi::from_hex(hex);
        }
    }


    namespace palettes
    {
        const Palette&
        dawnbringer()
        {
            static const auto p = make_static_palette
            (
                "Dawnbringer",
                hex(0x140C1C),
                hex(0x442434),
                hex(0x30346D),
                hex(0x4E4A4E),
                hex(0x854C30),
                hex(0x346524),
                hex(0xD04648),
                hex(0x757161),
                hex(0x597DCE),
                hex(0xD27D2C),
                hex(0x8595A1),
                hex(0x6DAA2C),
                hex(0xD2AA99),
                hex(0x6DC2CA),
                hex(0xDAD45E),
                hex(0xDEEED6)
            );
            return *p;
        }

        const Palette&
        named_colors()
        {
            static const auto p = make_static_palette
            (
                "Named colors",
                Rgbi{NamedColor::white},
                Rgbi{NamedColor::light_gray},
                Rgbi{NamedColor::gray},
                Rgbi{NamedColor::dark_gray},
                Rgbi{NamedColor::black},
                Rgbi{NamedColor::red},
                Rgbi{NamedColor::pure_red},
                Rgbi{NamedColor::blue},
                Rgbi{NamedColor::pure_blue},
                Rgbi{NamedColor::light_blue},
                Rgbi{NamedColor::normal_blue},
                Rgbi{NamedColor::cornflower_blue},
                Rgbi{NamedColor::green},
                Rgbi{NamedColor::pure_green},
                Rgbi{NamedColor::light_green},
                Rgbi{NamedColor::yellow},
                Rgbi{NamedColor::pure_yellow},
                Rgbi{NamedColor::orange},
                Rgbi{NamedColor::pure_orange},
                Rgbi{NamedColor::brown},
                Rgbi{NamedColor::pure_brown},
                Rgbi{NamedColor::purple},
                Rgbi{NamedColor::pure_purple},
                Rgbi{NamedColor::pink},
                Rgbi{NamedColor::pure_pink},
                Rgbi{NamedColor::pure_beige},
                Rgbi{NamedColor::tan},
                Rgbi{NamedColor::pure_tan},
                Rgbi{NamedColor::cyan},
                Rgbi{NamedColor::pure_cyan}
            );
            return *p;
        }
    }
}

