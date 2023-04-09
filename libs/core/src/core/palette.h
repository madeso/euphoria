#pragma once


#include <string_view>
#include <array>
#include "range/v3/view/span.hpp"

#include "core/rgb.h"

namespace euphoria::core
{
    struct Random;

    struct Palette
    {
        // todo(Gustav): what is name used for? can we remove it and use a tostring on the palette_all enum?
        // also does this represent a display name or a lookup name? both?
        /** the name of the palette */
        std::string_view name;

        ranges::span<const Rgbi> colors;

        constexpr Palette(const std::string_view& n, const ranges::span<const Rgbi>& c)
            : name(n)
            , colors(c)
        {
        }

        /** Get a random color.
         * @param r the random generator to use
         * @returns a random color
         */
        const Rgbi&
        get_random_color(Random* r) const;

        /** Get a color based on the index.
         * The index is modulated based on the palette size, ensuring a valid color.
         * @param i the index
         * @returns the color
        */
        [[nodiscard]] const Rgbi&
        get_safe_index(unsigned int i) const;

        [[nodiscard]] const Rgbi&
        get_closest_color(const Rgbi& c) const;

        [[nodiscard]] int
        get_index_closest(const Rgbi& c) const;

        // todo(Gustav): provide a function that takes a (void) lambda
        // returing a index, and we return a (safe) color from that index
        // with this construct we can simplyfy switch() statements
        // that needs to determine a color
    };


    template<std::size_t size>
    struct StaticPalette
    {
        std::string_view name;
        std::array<const Rgbi, size> colors;
        Palette pal;

        constexpr StaticPalette
        (
            std::string_view n,
            const std::array<const Rgbi, size>& c
        )
            : name(n), colors(c), pal{n, colors}
        {
        }

        constexpr const Palette& operator*() const
        {
            return pal;
        }

        constexpr const Palette* operator->() const
        {
            return &pal;
        }
    };


    template <typename... T>
    constexpr StaticPalette<sizeof...(T)>
    make_static_palette
    (
        const std::string_view& name,
        T... colors
    )
    {
        return {name, {colors...}};
    }

    struct DynamicPalette
    {
        std::string name;

        /** list of the colors */
        std::vector<Rgbi> colors;

        template <typename... R>
        DynamicPalette(const std::string& n, const Rgbi& c0, const R&... c)
            : name(n)
            , colors {c0, c...}
        {
        }

        DynamicPalette(const std::string& n, const std::vector<Rgbi>& c);

        /** Create a empty palette with a name.
         * @param name the name of the palette
         * @returns a empty palette
         */
        [[nodiscard]] static DynamicPalette
        create_empty(const std::string& name);

        /// based on the r documentation https://rdrr.io/r/grDevices/palettes.html
        [[nodiscard]] static DynamicPalette
        create_rainbow(int count, float saturation = 0.5f, float lightness = 0.5f);

        /// based on the r documentation https://rdrr.io/r/grDevices/palettes.html
        [[nodiscard]] static DynamicPalette create_rainbow
        (
            int count,
            const Angle& from,
            const Angle& to,
            float saturation,
            float lightness
        );

        [[nodiscard]] Palette to_palette() const;

    private:
        explicit DynamicPalette(const std::string& n);
    };


    // http://pixeljoint.com/forum/forum_posts.asp?TID=12795
    constexpr auto dawnbringer_palette = make_static_palette
    (
        "Dawnbringer",
        Rgbi::from_hex(0x140C1C),
        Rgbi::from_hex(0x442434),
        Rgbi::from_hex(0x30346D),
        Rgbi::from_hex(0x4E4A4E),
        Rgbi::from_hex(0x854C30),
        Rgbi::from_hex(0x346524),
        Rgbi::from_hex(0xD04648),
        Rgbi::from_hex(0x757161),
        Rgbi::from_hex(0x597DCE),
        Rgbi::from_hex(0xD27D2C),
        Rgbi::from_hex(0x8595A1),
        Rgbi::from_hex(0x6DAA2C),
        Rgbi::from_hex(0xD2AA99),
        Rgbi::from_hex(0x6DC2CA),
        Rgbi::from_hex(0xDAD45E),
        Rgbi::from_hex(0xDEEED6)
    );

    constexpr auto named_colors_palette = make_static_palette
    (
        "Named colors",
        Rgbi{ NamedColor::white },
        Rgbi{ NamedColor::light_gray },
        Rgbi{ NamedColor::gray },
        Rgbi{ NamedColor::dark_gray },
        Rgbi{ NamedColor::black },
        Rgbi{ NamedColor::red },
        Rgbi{ NamedColor::pure_red },
        Rgbi{ NamedColor::blue },
        Rgbi{ NamedColor::pure_blue },
        Rgbi{ NamedColor::light_blue },
        Rgbi{ NamedColor::normal_blue },
        Rgbi{ NamedColor::cornflower_blue },
        Rgbi{ NamedColor::green },
        Rgbi{ NamedColor::pure_green },
        Rgbi{ NamedColor::light_green },
        Rgbi{ NamedColor::yellow },
        Rgbi{ NamedColor::pure_yellow },
        Rgbi{ NamedColor::orange },
        Rgbi{ NamedColor::pure_orange },
        Rgbi{ NamedColor::brown },
        Rgbi{ NamedColor::pure_brown },
        Rgbi{ NamedColor::purple },
        Rgbi{ NamedColor::pure_purple },
        Rgbi{ NamedColor::pink },
        Rgbi{ NamedColor::pure_pink },
        Rgbi{ NamedColor::pure_beige },
        Rgbi{ NamedColor::tan },
        Rgbi{ NamedColor::pure_tan },
        Rgbi{ NamedColor::cyan },
        Rgbi{ NamedColor::pure_cyan }
    );

}
