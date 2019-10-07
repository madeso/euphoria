#ifndef EUPHORIA_PALETTE_H
#define EUPHORIA_PALETTE_H

#include <string>

#include "core/rgb.h"

namespace euphoria::core
{
    class Random;

    struct Palette
    {
        /** the name of the palette */
        std::string name;

        /** list of the colors */
        std::vector<Rgbi> colors;

        template <typename... R>
        Palette(const std::string& n, const Rgbi& c0, const R&... c)
            : name(n), colors {c0, c...}
        {}

        /** Create a empty palette with a name.
    @param name the name of the palette
    @returns a empty palette
    */
        static Palette
        Empty(const std::string& name);

        /** Get a random color.
   @param r the random generator to use
   @returns a random color
   */
        const Rgbi&
        GetRandomColor(Random* r) const;

        /** Get a color based on the index.
    The index is modulated based on the palette size, ensuring a valid color.
    @param i the index
    @returns the color
    */
        const Rgbi&
        GetSafeIndex(unsigned int i) const;

        const Rgbi&
        GetClosestColor(const Rgbi& c) const;

        unsigned int
        GetIndexClosest(const Rgbi& c) const;

        // rainbow functions based on the r documentation https://rdrr.io/r/grDevices/palettes.html

        static Palette
        Rainbow(int count, float saturation = 1, float lightness = 1);

        static Palette
        Rainbow(int          count,
                const Angle& from,
                const Angle& to,
                float        saturation,
                float        lightness);

    private:
        explicit Palette(const std::string& n);
    };

    namespace palette
    {
        // http://pixeljoint.com/forum/forum_posts.asp?TID=12795
        const Palette&
        Dawnbringer();

        const Palette&
        NamedColors();
    }  // namespace palette

}  // namespace euphoria::core

#endif  // EUPHORIA_PALETTE_H
