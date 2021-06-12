#pragma once

#include <string>
#include <string_view>
#include <array>
#include "range/v3/view/span.hpp"

#include "core/rgb.h"

namespace euphoria::core
{
    struct random;

    struct palette
    {
        // todo(Gustav): what is name used for? can we remove it and use a tostring on the palette_all enum?
        // also does this represent a display name or a lookup name? both?
        /** the name of the palette */
        std::string_view name;

        ranges::span<const rgbi> colors;

        constexpr palette(const std::string_view& n, const ranges::span<const rgbi>& c)
            : name(n)
            , colors(c)
        {
        }

        /** Get a random color.
         * @param r the random generator to use
         * @returns a random color
         */
        const rgbi&
        get_random_color(random* r) const;

        /** Get a color based on the index.
         * The index is modulated based on the palette size, ensuring a valid color.
         * @param i the index
         * @returns the color
        */
        [[nodiscard]] const rgbi&
        get_safe_index(unsigned int i) const;

        [[nodiscard]] const rgbi&
        get_closest_color(const rgbi& c) const;

        [[nodiscard]] unsigned int
        get_index_closest(const rgbi& c) const;

        // todo(Gustav): provide a function that takes a (void) lambda
        // returing a index, and we return a (safe) color from that index
        // with this construct we can simplyfy switch() statements
        // that needs to determine a color
    };


    template<std::size_t size>
    struct static_palette
    {
        std::string_view name;
        std::array<const rgbi, size> colors;
        palette pal;

        constexpr static_palette
        (
            std::string_view n,
            const std::array<const rgbi, size>& c
        )
            : name(n), colors(c), pal{n, colors}
        {
        }

        constexpr const palette& operator*() const
        {
            return pal;
        }

        constexpr const palette* operator->() const
        {
            return &pal;
        }
    };


    template <typename... T>
    constexpr static_palette<sizeof...(T)>
    make_static_palette
    (
        const std::string_view& name,
        T... colors
    )
    {
        return {name, {colors...}};
    }

    struct dynamic_palette
    {
        std::string name;

        /** list of the colors */
        std::vector<rgbi> colors;

        palette
        to_palette() const;

        template <typename... R>
        dynamic_palette(const std::string& n, const rgbi& c0, const R&... c)
            : name(n)
            , colors {c0, c...}
        {
        }

        dynamic_palette(const std::string& n, const std::vector<rgbi>& c);

        /** Create a empty palette with a name.
         * @param name the name of the palette
         * @returns a empty palette
         */
        [[nodiscard]]
        static
        dynamic_palette
        create_empty(const std::string& name);

        // rainbow functions based on the r documentation https://rdrr.io/r/grDevices/palettes.html

        [[nodiscard]]
        static
        dynamic_palette
        create_rainbow(int count, float saturation = 0.5f, float lightness = 0.5f);

        [[nodiscard]]
        static
        dynamic_palette
        create_rainbow
        (
            int count,
            const angle& from,
            const angle& to,
            float saturation,
            float lightness
        );

    private:
        explicit dynamic_palette(const std::string& n);
    };


    namespace palettes
    {
        // http://pixeljoint.com/forum/forum_posts.asp?TID=12795
        const palette&
        dawnbringer();

        const palette&
        named_colors();
    }

}
