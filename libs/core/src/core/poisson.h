#pragma once

#include <vector>
#include <tuple>
#include <optional>

#include "core/vec2.h"
#include "core/rect.h"
#include "core/table.h"

namespace euphoria::core
{
    struct Random;

    struct poisson_worker
    {
        poisson_worker(const Rectf& area, Random* random, float r, float bs, int k);

        Rectf area;
        Random* random;
        float r;
        float bounds_check;
        int k;

        float w;
        table<int> grid;
        std::vector<int> active;
        std::vector<vec2f> samples;

        [[nodiscard]] bool
        is_done() const;

        std::optional<std::tuple<vec2f, vec2f>>
        step();

        [[nodiscard]] vec2f random_point() const;
        [[nodiscard]] vec2i point_to_index(const vec2f& p) const;

        bool
        can_place_at(const vec2f& potential_sample, const vec2i& potential_sample_pos);

        std::tuple<bool, vec2f>
        try_place(int active_index);
    };

    // r = minimum distance
    std::vector<vec2f> poisson_sample(const Rectf& area, Random* random, float r, float bs=-1.0f, int k=30);
}

