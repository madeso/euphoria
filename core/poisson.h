#ifndef EUPHORIA_CORE_POISSON_H
#define EUPHORIA_CORE_POISSON_H

#include <vector>

#include "core/vec2.h"
#include "core/rect.h"
#include "core/table.h"

namespace euphoria::core
{
    struct Random;

    struct PoissonWorker
    {
        PoissonWorker(const Rectf& area, Random* random, float r, int k);

        Rectf area;
        Random* random;
        float r;
        int k;

        float w;
        Table<int> grid;
        std::vector<int> active;
        std::vector<vec2f> samples;

        bool
        IsDone() const;

        void
        Step();

        vec2f random_point();
        vec2i point_to_index(const vec2f& p);

        bool can_place_at(const vec2f& sample, const vec2i& sample_index);
        bool try_place(int active_index);
    };

    // r = minimum distance
    std::vector<vec2f> PoissonSample(const Rectf& area, Random* random, float r, int k=30);
}

#endif  // EUPHORIA_CORE_POISSON_H
