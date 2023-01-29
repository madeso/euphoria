#pragma once



#include "core/rgb.h"


namespace euphoria::core
{
    struct Image;

    std::vector<rgbi>
    extract_palette_median_cut(const Image& image, int depth, bool middle_split);
}

