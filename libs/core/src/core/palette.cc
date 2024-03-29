#include "core/palette.h"

#include "assert/assert.h"
#include "base/numeric.h"
#include "base/random.h"
#include "base/colors.h"


namespace eu::core
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
    Palette::get_safe_index(unsigned int index) const
    {
        return colors[index % colors.size()];
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

        for(int color_index = 0; color_index < count; color_index += 1)
        {
            float d = static_cast<float>(color_index) / static_cast<float>(count - 1);
            const auto rgbf = to_rgb
            (
                Hsl
                {
                    lerp_angle(from, d, to),
                    saturation,
                    lightness
                }
            );
            pal.colors.push_back(to_rgbi(rgbf));
        }

        return pal;
    }


    DynamicPalette::DynamicPalette(const std::string& n)
        : name(n)
    {
    }

}

