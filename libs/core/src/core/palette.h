#ifndef EUPHORIA_PALETTE_H
#define EUPHORIA_PALETTE_H

#include <string>

#include "core/rgb.h"

namespace euphoria::core
{
    struct Random;

    struct palette
    {
        /** the name of the palette */
        std::string name;

        /** list of the colors */
        std::vector<rgbi> colors;

        template <typename... R>
        palette(const std::string& n, const rgbi& c0, const R&... c)
            : name(n)
            , colors {c0, c...}
        {
        }

        palette(const std::string& n, const std::vector<rgbi>& c);

        /** Create a empty palette with a name.
         * @param name the name of the palette
         * @returns a empty palette
         */
        [[nodiscard]]
        static
        palette
        create_empty(const std::string& name);

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

        [[nodiscard]] unsigned int
        get_index_closest(const rgbi& c) const;

        // todo(Gustav): provide a function that takes a (void) lambda
        // returing a index, and we return a (safe) color from that index
        // with this construct we can simplyfy switch() statements
        // that needs to determine a color

        // rainbow functions based on the r documentation https://rdrr.io/r/grDevices/palettes.html

        [[nodiscard]]
        static
        palette
        create_rainbow(int count, float saturation = 0.5f, float lightness = 0.5f);

        [[nodiscard]]
        static
        palette
        create_rainbow
        (
            int count,
            const angle& from,
            const angle& to,
            float saturation,
            float lightness
        );

    private:
        explicit palette(const std::string& n);
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

#endif  // EUPHORIA_PALETTE_H
