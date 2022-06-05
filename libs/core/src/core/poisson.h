#pragma once


#include <tuple>
#include <optional>

#include "core/vec2.h"
#include "core/rect.h"
#include "core/table.h"

namespace euphoria::core
{
    struct Random;

    struct PoissonWorker
    {
        PoissonWorker(const Rectf& area, Random* random, float r, float bs, int k);

        Rectf area;
        Random* rand;
        float r;
        float bounds_check;
        int k;

        float w;
        Table<int> grid;
        std::vector<int> active;
        std::vector<Vec2f> samples;

        [[nodiscard]] bool
        is_done() const;

        std::optional<std::tuple<Vec2f, Vec2f>>
        step();

        [[nodiscard]] Vec2f random_point() const;
        [[nodiscard]] Vec2i point_to_index(const Vec2f& p) const;

        bool
        can_place_at(const Vec2f& potential_sample, const Vec2i& potential_sample_pos);

        std::tuple<bool, Vec2f>
        try_place(int active_index);
    };

    // r = minimum distance
    std::vector<Vec2f> poisson_sample(const Rectf& area, Random* random, float r, float bs=-1.0f, int k=30);
}

