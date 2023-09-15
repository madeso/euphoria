#include "core/image_to_text.h"

#include <iostream>

#include "core/image.h"
#include "core/palette.h"
#include "core/stringbuilder.h"


namespace eu::core
{
    Table<char>
    from_image_to_string_table(const Image& img, const std::vector<ImageMapAction>& map)
    {
        auto pal = DynamicPalette::create_empty("");
        for(const auto m: map)
        {
            pal.colors.push_back(m.from_color);
        }

        auto ret = Table<char>::from_width_height
        (
            img.width,
            img.height,
            ' '
        );
        ret.set_all
        (
            [&pal, &map, &img](int x, int y)
            {
                const auto p = img.get_pixel(x, y);
                const auto index = pal.to_palette().get_index_closest(Rgbi{p.r, p.g, p.b});
                return map[index].to;
            }
        );

        return ret;
    }

    Table<char>
    from_image_to_string_table_exact
    (
        const Image& img,
        const std::vector<ImageMapAction>& map, char missing
    )
    {
        auto find_match = [&](const Rgbi& c) -> char
        {
            for(const auto& m: map)
            {
                if(m.from_color == c)
                {
                    return m.to;
                }
            }

            return missing;
        };
        auto ret = Table<char>::from_width_height(img.width, img.height, ' ');
        ret.set_all([&](int x, int y) {
            const auto p = img.get_pixel(x, y);
            const auto c = Rgbi {p.r, p.g, p.b};
            const auto r = find_match(c);
            return r;
        });

        return ret;
    }

    Table<char>
    from_image_to_string_table(const Image& img, bool shorter, Grayscale grayscale)
    {
        auto ret = Table<char>::from_width_height(img.width, img.height, ' ');
        ret.set_all([shorter, &img, grayscale](int x, int y) {
            // http://paulbourke.net/dataformats/asciiart/
            const std::string characters = shorter
                ? "@%#*+=-:. "
                : "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. "
                ;
            const auto inverted_color = to_rgb
            (
                make_grayscale(img.get_pixel(x, y), grayscale)
            ).r;
            const auto p = 1.0f - inverted_color;
            const auto index = floor_to_int(p * static_cast<float>(characters.size() - 1) );
            return characters[index];
        });

        return ret;
    }

    std::vector<std::string>
    to_strings(const Table<char>& table)
    {
        std::vector<std::string> ret;

        for(int y = 0; y < table.get_height(); y += 1)
        {
            StringBuilder ss;
            for(int x = 0; x < table.get_width(); x += 1)
            {
                ss.add_char(table[{x, y}]);
            }
            ret.insert(ret.begin(), ss.to_string());
        }

        return ret;
    }

}
