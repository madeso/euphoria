#ifndef CORE_GENERATOR_MAZE_H
#define CORE_GENERATOR_MAZE_H

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
    namespace Cell
    {
        enum Type
        {
            None      = 0,
            PathNorth = 1 << 0,
            PathSouth = 1 << 1,
            PathEast  = 1 << 2,
            PathWest  = 1 << 3,
            Visited   = 1 << 4
        };
    }

    enum class Dir
    {
        North,
        South,
        East,
        West
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

        virtual void
        Setup() = 0;

        [[nodiscard]] virtual bool
        HasMoreWork() const = 0;

        virtual void
        Work() = 0;
    };

    struct RecursiveBacktracker : public Algorithm
    {
        Maze*   maze   = nullptr;
        Random* random = nullptr;

        std::stack<vec2i> stack;
        int               visited_cells = 0;

        void
        Setup() override;

        [[nodiscard]] bool
        HasMoreWork() const override;
        void
        Work() override;
    };

    struct RandomTraversal : public Algorithm
    {
        Maze*   maze   = nullptr;
        Random* random = nullptr;

        struct Entry
        {
            vec2i position;
            Dir   direction;
        };
        std::vector<Entry> frontier;

        void
        Setup() override;

        [[nodiscard]] bool
        HasMoreWork() const override;

        void
        Work() override;
    };

    struct Drawer
    {
        Maze* maze = nullptr;

        int cell_size = 3;
        int wall_size = 1;

        RecursiveBacktracker* tracker   = nullptr;
        RandomTraversal*      traversal = nullptr;

        rgbi wall_color;
        rgbi cell_color;
        rgbi cell_visited_color;
        rgbi unit_color;
        rgbi corridor_color;

        [[nodiscard]] rgbi
        CalculateCellColor(int x, int y) const;

        Image image;

        Drawer();

        void
        Draw();
    };
}  // namespace euphoria::core::generator


#endif  // CORE_GENERATOR_MAZE_H
