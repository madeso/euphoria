#ifndef EUPHORIA_CORE_POISSON_H
#define EUPHORIA_CORE_POISSON_H

#include <vector>

#include "core/vec2.h"
#include "core/rect.h"

namespace euphoria::core
{
    struct Random;

    // r = minimum distance
    std::vector<vec2f> PoissonSample(const Rectf& area, Random* random, float r, int k=30);
}

#endif  // EUPHORIA_CORE_POISSON_H
