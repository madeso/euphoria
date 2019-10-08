#include "core/imageops.h"

#include "core/image.h"
#include "core/palette.h"

namespace euphoria::core
{
    Table<char>
    ImageToStringTable(const Image& img, const std::vector<ImageMapAction>& map)
    {
        auto pal = Palette::Empty("");
        for(const auto m: map)
        {
            pal.colors.push_back(m.from_color);
        }

        auto ret = Table<char>::FromWidthHeight(
                img.GetWidth(), img.GetHeight(), ' ');
        ret.SetAll([&pal, &map, &img](int x, int y) {
            const auto p     = img.GetPixel(x, y);
            const auto index = pal.GetIndexClosest(Rgbi {p.r, p.g, p.b});
            return map[index].to;
        });

        return ret;
    }

    Table<char>
    ImageToStringTable(const Image& img, bool shorter)
    {
        auto ret = Table<char>::FromWidthHeight(
                img.GetWidth(), img.GetHeight(), ' ');
        ret.SetAll([shorter, &img](int x, int y) {
            // http://paulbourke.net/dataformats/asciiart/
            const std::string characters
                    = shorter ? " .:-=+*#%@"
                              : "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
            const auto p     = 1.0f - rgb(img.GetPixel(x, y)).r;
            const auto index = Floori(p * (characters.size() - 1));
            return characters[index];
        });

        return ret;
    }

}  // namespace euphoria::core
