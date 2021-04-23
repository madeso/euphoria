#pragma once

#include "core/table.h"
#include "core/vec2.h"
#include "core/image.h"
#include "core/enumtostring.h"

#include <stack>

namespace euphoria::core
{
    struct Random;
}

namespace euphoria::core::generator
{
    namespace cell
    {
        enum type
        {
            none      = 0,
            path_north = 1 << 0,
            path_south = 1 << 1,
            path_east  = 1 << 2,
            path_west  = 1 << 3,
            visited   = 1 << 4
        };
    }

    enum class dir
    {
        north,
        south,
        east,
        west
    };

    using maze = table<int>;

    // todo(Gustav): implement more generators
    // https://bost.ocks.org/mike/algorithms/#maze-generation
    // https://gamedev.stackexchange.com/questions/75632/procedural-generation-of-tile-based-2d-world
    // https://unity3d.com/learn/tutorials/s/procedural-cave-generation-tutorial
    // https://gamedev.stackexchange.com/questions/153734/generate-cave-like-terrain-in-2d
    // http://www.gamasutra.com/blogs/HermanTulleken/20161005/282629/Algorithms_for_making_more_interesting_mazes.php

    struct algorithm
    {
        algorithm() = default;
        virtual ~algorithm() = default;

        algorithm(const algorithm&) = delete;
        algorithm(algorithm&&) = delete;
        void operator=(const algorithm&) = delete;
        void operator=(algorithm&&) = delete;

        virtual void
        setup() = 0;

        [[nodiscard]] virtual bool
        has_more_work() const = 0;

        virtual void
        work() = 0;
    };

    struct recursive_backtracker : public algorithm
    {
        generator::maze*   maze   = nullptr;
        Random* random = nullptr;

        std::stack<vec2i> stack;
        int               visited_cells = 0;

        void
        setup() override;

        [[nodiscard]] bool
        has_more_work() const override;

        void
        work() override;
    };

    struct random_traversal : public algorithm
    {
        generator::maze*   maze   = nullptr;
        Random* random = nullptr;

        struct entry
        {
            vec2i position;
            dir   direction;
        };
        std::vector<entry> frontier;

        void
        setup() override;

        [[nodiscard]] bool
        has_more_work() const override;

        void
        work() override;
    };

    struct drawer
    {
        generator::maze* maze = nullptr;

        int cell_size = 3;
        int wall_size = 1;

        recursive_backtracker* tracker   = nullptr;
        random_traversal*      traversal = nullptr;

        rgbi wall_color;
        rgbi cell_color;
        rgbi cell_visited_color;
        rgbi unit_color;
        rgbi corridor_color;

        [[nodiscard]] rgbi
        calculate_cell_color(int x, int y) const;

        core::image image;

        drawer();

        void
        draw();
    };
}
