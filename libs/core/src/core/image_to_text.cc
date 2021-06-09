#include "core/image_to_text.h"

#include <iostream>

#include "core/image.h"
#include "core/palette.h"

namespace euphoria::core
{
    table<char>
    image_to_string_table(const image& img, const std::vector<image_map_action>& map)
    {
        auto pal = dynamic_palette::create_empty("");
        for(const auto m: map)
        {
            pal.colors.push_back(m.from_color);
        }

        auto ret = table<char>::from_width_height
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
                const auto index = pal.to_palette().get_index_closest(rgbi {p.r, p.g, p.b});
                return map[index].to;
            }
        );

        return ret;
    }

    table<char>
    image_to_string_table_exact(
            const image& img,
            const std::vector<image_map_action>& map, char missing)
    {
        auto find_match = [&](const rgbi& c) -> char
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
        auto ret = table<char>::from_width_height(
                img.width, img.height, ' ');
        ret.set_all([&](int x, int y) {
            const auto p = img.get_pixel(x, y);
            const auto c = rgbi {p.r, p.g, p.b};
            const auto r = find_match(c);
            return r;
        });

        return ret;
    }

    table<char>
    image_to_string_table(const image& img, bool shorter, grayscale grayscale)
    {
        auto ret = table<char>::from_width_height(
                img.width, img.height, ' ');
        ret.set_all([shorter, &img, grayscale](int x, int y) {
            // http://paulbourke.net/dataformats/asciiart/
            const std::string characters = shorter
                ? "@%#*+=-:. "
                : "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
            const auto inverted_color = crgb
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
    to_strings(const table<char>& table)
    {
        std::vector<std::string> ret;

        for(int r = 0; r < table.get_height(); r += 1)
        {
            std::stringstream ss;
            for(int c = 0; c < table.get_width(); c += 1)
            {
                ss << table(c, r);
            }
            ret.insert(ret.begin(), ss.str());
        }

        return ret;
    }

}
