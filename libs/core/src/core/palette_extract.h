#ifndef EUPHORIA_CORE_PALETTE_EXTRACT_H
#define EUPHORIA_CORE_PALETTE_EXTRACT_H

#include <vector>

#include "core/rgb.h"


namespace euphoria::core
{
    struct Image;

    std::vector<rgbi>
    MedianCut(const Image& image, int depth, bool middle_split);
}

#endif  // EUPHORIA_CORE_PALETTE_EXTRACT_H

