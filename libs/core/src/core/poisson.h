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
        Rectf area;
        Random* rand;
        float r;
        float bounds_check;
        int k;

        float w;
        Table<int> grid;
        std::vector<int> active;
        std::vector<vec2f> samples;

        PoissonWorker(const Rectf& area, Random* random, float r, float bs, int k);

        std::optional<std::tuple<vec2f, vec2f>> step();
        bool can_place_at(const vec2f& potential_sample, const vec2i& potential_sample_pos);
        std::tuple<bool, vec2f> try_place(int active_index);

        [[nodiscard]] vec2i c_point_to_index(const vec2f& p) const;

        [[nodiscard]] vec2f get_random_point_in_area() const;
        [[nodiscard]] bool is_done() const;
    };

    // r = minimum distance
    std::vector<vec2f> calc_poisson_samples(const Rectf& area, Random* random, float r, float bs=-1.0f, int k=30);
}

