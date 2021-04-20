#include "core/imageops.h"

#include <iostream>

#include "core/image.h"
#include "core/palette.h"

namespace euphoria::core
{
    Table<char>
    ImageToStringTable(const image& img, const std::vector<ImageMapAction>& map)
    {
        auto pal = Palette::Empty("");
        for(const auto m: map)
        {
            pal.colors.push_back(m.from_color);
        }

        auto ret = Table<char>::FromWidthHeight(
                img.width, img.height, ' ');
        ret.SetAll([&pal, &map, &img](int x, int y) {
            const auto p     = img.get_pixel(x, y);
            const auto index = pal.GetIndexClosest(rgbi {p.r, p.g, p.b});
            return map[index].to;
        });

        return ret;
    }

    Table<char>
    ImageToStringTableExact(
            const image&                       img,
            const std::vector<ImageMapAction>& map, char missing)
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
        auto ret = Table<char>::FromWidthHeight(
                img.width, img.height, ' ');
        ret.SetAll([&](int x, int y) {
            const auto p = img.get_pixel(x, y);
            const auto c = rgbi {p.r, p.g, p.b};
            const auto r = find_match(c);
            return r;
        });

        return ret;
    }

    Table<char>
    ImageToStringTable(const image& img, bool shorter, Grayscale grayscale)
    {
        auto ret = Table<char>::FromWidthHeight(
                img.width, img.height, ' ');
        ret.SetAll([shorter, &img, grayscale](int x, int y) {
            // http://paulbourke.net/dataformats/asciiart/
            const std::string characters = shorter
                ? "@%#*+=-:. "
                : "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
            const auto inverted_color = crgb
            (
                MakeGrayscale(img.get_pixel(x, y), grayscale)
            ).r;
            const auto p = 1.0f - inverted_color;
            const auto index = Floori(p * static_cast<float>(characters.size() - 1) );
            return characters[index];
        });

        return ret;
    }

    std::vector<std::string>
    ToStrings(const Table<char>& table)
    {
        std::vector<std::string> ret;

        for(int r = 0; r < table.GetHeight(); r += 1)
        {
            std::stringstream ss;
            for(int c = 0; c < table.GetWidth(); c += 1)
            {
                ss << table(c, r);
            }
            ret.insert(ret.begin(), ss.str());
        }

        return ret;
    }

}  // namespace euphoria::core
