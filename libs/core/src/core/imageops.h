#ifndef CORE_IMAGEOPS_H
#define CORE_IMAGEOPS_H

#include <vector>
#include <string>

#include "core/table.h"
#include "core/rgb.h"

#include "core/imagefilters.h"

// todo(Gustav): Rename module to something like console image or something...

namespace euphoria::core
{
    struct Image;

    struct ImageMapAction
    {
        char to;
        Rgbi from_color;
    };

    Table<char>
    ImageToStringTable(
            const Image&                       img,
            const std::vector<ImageMapAction>& map);

    Table<char>
    ImageToStringTableExact(
            const Image&                       img,
            const std::vector<ImageMapAction>& map, char missing);

    Table<char>
    ImageToStringTable(const Image& img, bool shorter, Grayscale grayscale = Grayscale::R);

    std::vector<std::string>
    ToStrings(const Table<char>& t);

    // todo(Gustav): add function to print image using colored unicode blocks

}  // namespace euphoria::core

#endif  // CORE_IMAGEOPS_H
