#pragma once

#include <vector>

#include "core/rgb.h"


namespace euphoria::core
{
    struct image;

    std::vector<rgbi>
    median_cut(const image& image, int depth, bool middle_split);
}

