#include "core/palette.h"

#include "core/assert.h"
#include "core/numeric.h"
#include "core/random.h"
#include "core/colors.h"


namespace euphoria::core
{
    namespace
    {
        float
        Diff(const rgbi& lhs, const rgbi& rhs)
        {
            return
                Square(static_cast<float>(lhs.r - rhs.r)) +
                Square(static_cast<float>(lhs.g - rhs.g)) +
                Square(static_cast<float>(lhs.b - rhs.b))
                ;
        }
    }


    palette::palette(const std::string& n, const std::vector<rgbi>& c)
        : name(n)
        , colors(c)
    {
    }


    palette
    palette::create_empty(const std::string& name)
    {
        return palette{name};
    }


    const rgbi&
    palette::get_random_color(Random* r) const
    {
        return r->Next(colors);
    }


    const rgbi&
    palette::get_safe_index(unsigned int i) const
    {
        return colors[i % colors.size()];
    }


    const rgbi&
    palette::get_closest_color(const rgbi& c) const
    {
        return colors[get_index_closest(c)];
    }


    unsigned int
    palette::get_index_closest(const rgbi& c) const
    {
        ASSERT(!colors.empty());
        auto diff_best = Diff(c, colors[0]);

        unsigned int index_best = 0;
        const auto size = colors.size();
        for(unsigned int index = 1; index < size; index += 1)
        {
            const auto diff = Diff(c, colors[index]);
            if(diff < diff_best)
            {
                diff_best = diff;
                index_best = index;
            }
        }

        return index_best;
    }


    palette
    palette::create_rainbow(int count, float saturation, float lightness)
    {
        return create_rainbow
        (
            count,
            angle::from_radians(0),
            angle::from_percent_of_360
            (
                Max(1.0f, static_cast<float>(count - 1) / static_cast<float>(count))
            ),
            saturation,
            lightness
        );
    }


    palette
    palette::create_rainbow
    (
        int count,
        const angle& from,
        const angle& to,
        float saturation,
        float lightness
    )
    {
        ASSERT(count > 1);

        auto pal = palette::create_empty("Rainbow");

        for(int i = 0; i < count; i += 1)
        {
            float d = static_cast<float>(i) / static_cast<float>(count - 1);
            const auto rgbf = crgb
            (
                hsl
                {
                    angle_transform::transform(from, d, to),
                    saturation,
                    lightness
                }
            );
            pal.colors.push_back(crgbi(rgbf));
        }

        return pal;
    }


    palette::palette(const std::string& n)
        : name(n)
    {
    }


    ////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        rgbi
        C(unsigned int hex)
        {
            return rgbi::from_hex(hex);
        }
    }


    namespace palettes
    {
        const palette&
        dawnbringer()
        {
            static const auto p = palette
            {
                "dawnbringer",
                C(0x140C1C),
                C(0x442434),
                C(0x30346D),
                C(0x4E4A4E),
                C(0x854C30),
                C(0x346524),
                C(0xD04648),
                C(0x757161),
                C(0x597DCE),
                C(0xD27D2C),
                C(0x8595A1),
                C(0x6DAA2C),
                C(0xD2AA99),
                C(0x6DC2CA),
                C(0xDAD45E),
                C(0xDEEED6),
            };
            return p;
        }


        palette
        BuildNamedColors()
        {
            auto p = palette
            {
                "named",
                color::white,
                color::light_gray,
                color::gray,
                color::dark_gray,
                color::black,
                color::red,
                color::pure_red,
                color::blue,
                color::pure_blue,
                color::light_blue,
                color::normal_blue,
                color::cornflower_blue,
                color::green,
                color::pure_green,
                color::light_green,
                color::yellow,
                color::pure_yellow,
                color::orange,
                color::pure_orange,
                color::brown,
                color::pure_brown,
                color::purple,
                color::pure_purple,
                color::pink,
                color::pure_pink,
                color::pure_beige,
                color::tan,
                color::pure_tan,
                color::cyan,
                color::pure_cyan,
            };
            return p;
        }


        const palette&
        named_colors()
        {
            static const auto p = BuildNamedColors();
            return p;
        }
    }
}

