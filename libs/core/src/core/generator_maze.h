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
        enum Type
        {
            none = 0,
            path_north = 1 << 0,
            path_south = 1 << 1,
            path_east = 1 << 2,
            path_west = 1 << 3,
            visited = 1 << 4
        };
    }

    enum class Direction
    {
        north,
        south,
        east,
        west
    };

    using Maze = Table<int>;

    // todo(Gustav): implement more generators
    // https://bost.ocks.org/mike/algorithms/#maze-generation
    // https://gamedev.stackexchange.com/questions/75632/procedural-generation-of-tile-based-2d-world
    // https://unity3d.com/learn/tutorials/s/procedural-cave-generation-tutorial
    // https://gamedev.stackexchange.com/questions/153734/generate-cave-like-terrain-in-2d
    // http://www.gamasutra.com/blogs/HermanTulleken/20161005/282629/Algorithms_for_making_more_interesting_mazes.php

    struct Algorithm
    {
        Algorithm() = default;
        virtual ~Algorithm() = default;

        Algorithm(const Algorithm&) = delete;
        Algorithm(Algorithm&&) = delete;
        void operator=(const Algorithm&) = delete;
        void operator=(Algorithm&&) = delete;

        virtual void setup() = 0;
        virtual void work() = 0;
        [[nodiscard]] virtual bool has_more_work() const = 0;
    };

    struct RecursiveBacktracker : public Algorithm
    {
        generator::Maze* maze = nullptr;
        core::Random* random = nullptr;

        std::stack<vec2i> stack;
        int visited_cells = 0;

        void setup() override;
        void work() override;
        [[nodiscard]] bool has_more_work() const override;
    };

    struct RandomTraversal : public Algorithm
    {
        generator::Maze* maze = nullptr;
        core::Random* random = nullptr;

        struct Entry
        {
            vec2i position;
            Direction direction;
        };
        std::vector<Entry> frontier;

        void setup() override;
        void work() override;
        [[nodiscard]] bool has_more_work() const override;
    };

    struct Drawer
    {
        generator::Maze* maze = nullptr;

        int cell_size = 3;
        int wall_size = 1;

        RecursiveBacktracker* tracker = nullptr;
        RandomTraversal* traversal = nullptr;

        rgbi wall_color;
        rgbi cell_color;
        rgbi cell_visited_color;
        rgbi unit_color;
        rgbi corridor_color;

        core::Image image;

        Drawer();

        void draw();

        [[nodiscard]] rgbi calculate_cell_color(int x, int y) const;
    };
}
