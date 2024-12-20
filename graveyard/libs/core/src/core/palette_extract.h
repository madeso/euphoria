#pragma once



#include "base/rgb.h"


namespace eu::core
{
    struct Image;

    std::vector<Rgbi>
    extract_palette_median_cut(const Image& image, int depth, bool middle_split);
}

