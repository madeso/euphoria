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
                square(static_cast<float>(lhs.r - rhs.r)) +
                square(static_cast<float>(lhs.g - rhs.g)) +
                square(static_cast<float>(lhs.b - rhs.b))
                ;
        }
    }

    const rgbi&
    palette::get_random_color(random* r) const
    {
        return get_random_item_in_vector(r, colors);
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

    ///////////////////////////////////////////////////////////////////////////

    palette
    dynamic_palette::to_palette() const
    {
        return {name, colors};
    }


    dynamic_palette::dynamic_palette(const std::string& n, const std::vector<rgbi>& c)
        : name(n)
        , colors(c)
    {
    }


    dynamic_palette
    dynamic_palette::create_empty(const std::string& name)
    {
        return dynamic_palette{name};
    }


    dynamic_palette
    dynamic_palette::create_rainbow(int count, float saturation, float lightness)
    {
        return create_rainbow
        (
            count,
            angle::from_radians(0),
            angle::from_percent_of_360
            (
                max(1.0f, static_cast<float>(count - 1) / static_cast<float>(count))
            ),
            saturation,
            lightness
        );
    }


    dynamic_palette
    dynamic_palette::create_rainbow
    (
        int count,
        const angle& from,
        const angle& to,
        float saturation,
        float lightness
    )
    {
        ASSERT(count > 1);

        auto pal = dynamic_palette::create_empty("Rainbow");

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


    dynamic_palette::dynamic_palette(const std::string& n)
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
                std::array
                {
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
                }
            };
            return p;
        }

        const palette&
        named_colors()
        {
            static const auto p = palette
            {
                "named",
                std::array
                {
                    rgbi{color::white},
                    rgbi{color::light_gray},
                    rgbi{color::gray},
                    rgbi{color::dark_gray},
                    rgbi{color::black},
                    rgbi{color::red},
                    rgbi{color::pure_red},
                    rgbi{color::blue},
                    rgbi{color::pure_blue},
                    rgbi{color::light_blue},
                    rgbi{color::normal_blue},
                    rgbi{color::cornflower_blue},
                    rgbi{color::green},
                    rgbi{color::pure_green},
                    rgbi{color::light_green},
                    rgbi{color::yellow},
                    rgbi{color::pure_yellow},
                    rgbi{color::orange},
                    rgbi{color::pure_orange},
                    rgbi{color::brown},
                    rgbi{color::pure_brown},
                    rgbi{color::purple},
                    rgbi{color::pure_purple},
                    rgbi{color::pink},
                    rgbi{color::pure_pink},
                    rgbi{color::pure_beige},
                    rgbi{color::tan},
                    rgbi{color::pure_tan},
                    rgbi{color::cyan},
                    rgbi{color::pure_cyan}
                }
            };
            return p;
        }
    }
}

