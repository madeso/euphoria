#ifndef EUPHORIA_CORE_PALETTE_EXTRACT_H
#define EUPHORIA_CORE_PALETTE_EXTRACT_H

#include <vector>

#include "core/rgb.h"


namespace euphoria::core
{
    struct image;

    std::vector<rgbi>
    MedianCut(const image& image, int depth, bool middle_split);
}

#endif  // EUPHORIA_CORE_PALETTE_EXTRACT_H

