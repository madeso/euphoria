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

        ranges::span<const rgbi> colors;

        constexpr Palette(const std::string_view& n, const ranges::span<const rgbi>& c)
            : name(n)
            , colors(c)
        {
        }

        /** Get a random color.
         * @param r the random generator to use
         * @returns a random color
         */
        const rgbi&
        get_random_color(Random* r) const;

        /** Get a color based on the index.
         * The index is modulated based on the palette size, ensuring a valid color.
         * @param i the index
         * @returns the color
        */
        [[nodiscard]] const rgbi&
        get_safe_index(unsigned int i) const;

        [[nodiscard]] const rgbi&
        get_closest_color(const rgbi& c) const;

        [[nodiscard]] int
        get_index_closest(const rgbi& c) const;

        // todo(Gustav): provide a function that takes a (void) lambda
        // returing a index, and we return a (safe) color from that index
        // with this construct we can simplyfy switch() statements
        // that needs to determine a color
    };


    template<std::size_t size>
    struct StaticPalette
    {
        std::string_view name;
        std::array<const rgbi, size> colors;
        Palette pal;

        constexpr StaticPalette
        (
            std::string_view n,
            const std::array<const rgbi, size>& c
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
        std::vector<rgbi> colors;

        template <typename... R>
        DynamicPalette(const std::string& n, const rgbi& c0, const R&... c)
            : name(n)
            , colors {c0, c...}
        {
        }

        DynamicPalette(const std::string& n, const std::vector<rgbi>& c);

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
            const angle& from,
            const angle& to,
            float saturation,
            float lightness
        );

        [[nodiscard]] Palette to_palette() const;

    private:
        explicit DynamicPalette(const std::string& n);
    };


    namespace palettes
    {
        // http://pixeljoint.com/forum/forum_posts.asp?TID=12795
        constexpr auto dawnbringer = make_static_palette
        (
            "Dawnbringer",
            rgbi::from_hex(0x140C1C),
            rgbi::from_hex(0x442434),
            rgbi::from_hex(0x30346D),
            rgbi::from_hex(0x4E4A4E),
            rgbi::from_hex(0x854C30),
            rgbi::from_hex(0x346524),
            rgbi::from_hex(0xD04648),
            rgbi::from_hex(0x757161),
            rgbi::from_hex(0x597DCE),
            rgbi::from_hex(0xD27D2C),
            rgbi::from_hex(0x8595A1),
            rgbi::from_hex(0x6DAA2C),
            rgbi::from_hex(0xD2AA99),
            rgbi::from_hex(0x6DC2CA),
            rgbi::from_hex(0xDAD45E),
            rgbi::from_hex(0xDEEED6)
        );

        constexpr auto named_colors = make_static_palette
        (
            "Named colors",
            rgbi{ NamedColor::white },
            rgbi{ NamedColor::light_gray },
            rgbi{ NamedColor::gray },
            rgbi{ NamedColor::dark_gray },
            rgbi{ NamedColor::black },
            rgbi{ NamedColor::red },
            rgbi{ NamedColor::pure_red },
            rgbi{ NamedColor::blue },
            rgbi{ NamedColor::pure_blue },
            rgbi{ NamedColor::light_blue },
            rgbi{ NamedColor::normal_blue },
            rgbi{ NamedColor::cornflower_blue },
            rgbi{ NamedColor::green },
            rgbi{ NamedColor::pure_green },
            rgbi{ NamedColor::light_green },
            rgbi{ NamedColor::yellow },
            rgbi{ NamedColor::pure_yellow },
            rgbi{ NamedColor::orange },
            rgbi{ NamedColor::pure_orange },
            rgbi{ NamedColor::brown },
            rgbi{ NamedColor::pure_brown },
            rgbi{ NamedColor::purple },
            rgbi{ NamedColor::pure_purple },
            rgbi{ NamedColor::pink },
            rgbi{ NamedColor::pure_pink },
            rgbi{ NamedColor::pure_beige },
            rgbi{ NamedColor::tan },
            rgbi{ NamedColor::pure_tan },
            rgbi{ NamedColor::cyan },
            rgbi{ NamedColor::pure_cyan }
        );
    }

}
