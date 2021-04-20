#include "core/generator_maze.h"

#include "core/random.h"
#include "core/image_draw.h"
#include "core/colors.h"

namespace euphoria::core::generator
{
    vec2i
    DirToOffset(const Dir d)
    {
        switch(d)
        {
        case Dir::South: return vec2i { 0, -1};
        case Dir::North: return vec2i { 0,  1};
        case Dir::West:  return vec2i {-1,  0};
        case Dir::East:  return vec2i { 1,  0};
        default: return vec2i(0, 0);
        }
    }


    const std::vector<Dir>&
    AllDirs()
    {
        const static auto dirs = std::vector<Dir> {
                Dir::North, Dir::South, Dir::East, Dir::West};
        return dirs;
    }


    Dir
    FlipDirection(const Dir d)
    {
        switch(d)
        {
        case Dir::North: return Dir::South;
        case Dir::South: return Dir::North;
        case Dir::East: return Dir::West;
        case Dir::West: return Dir::East;
        default: return Dir::North;
        }
    }


    Cell::Type
    DirToCellPath(const Dir d)
    {
        switch(d)
        {
        case Dir::North: return Cell::PathNorth;
        case Dir::South: return Cell::PathSouth;
        case Dir::East: return Cell::PathEast;
        case Dir::West: return Cell::PathWest;
        default: return Cell::PathNorth;
        }
    }


    void
    Visit(Maze* maze, const vec2i& np)
    {
        (*maze)(np.x, np.y) |= Cell::Visited;
    }


    vec2i
    AddStepToMaze(Maze* maze, const vec2i& c, Dir dir)
    {
        const auto o  = DirToOffset(dir);
        const auto np = c + o;
        (*maze)(np.x, np.y)
                |= Cell::Visited | DirToCellPath(FlipDirection(dir));
        (*maze)(c.x, c.y) |= DirToCellPath(dir);
        return np;
    }


    bool
    HasVisited(Maze* maze, const vec2i& np)
    {
        return ((*maze)(np.x, np.y) & Cell::Visited) != 0;
    }


    bool
    CanVisitWithoutMakingLoop(Maze* maze, const vec2i& np)
    {
        const auto world_size = Recti::FromWidthHeight(
                maze->GetWidth() - 1, maze->GetHeight() - 1);
        return world_size.ContainsInclusive(np) && !HasVisited(maze, np);
    }


    template <typename T>
    T
    PopRandom(std::vector<T>* vec, Random* r)
    {
        ASSERT(!vec->empty());
        const auto i = r->NextRange(vec->size());
        T          t = (*vec)[i];
        vec->erase(vec->begin() + i);
        return t;
    }


    vec2i
    RandomPositionOnMaze(Random* random, Maze* maze)
    {
        return {random->NextRange(maze->GetWidth()),
                random->NextRange(maze->GetHeight())};
    }


    //////////////////////////////////////////////////////////////////////////////////////////

    void
    RecursiveBacktracker::Setup()
    {
        maze->Clear(Cell::None);

        const auto random_position = RandomPositionOnMaze(random, maze);

        stack.push(random_position);
        (*maze)(random_position.x, random_position.y) = Cell::Visited;
        visited_cells = 1;
    }


    bool
    RecursiveBacktracker::HasMoreWork() const
    {
        return visited_cells < maze->GetWidth() * maze->GetHeight();
    }


    void
    RecursiveBacktracker::Work()
    {
        const auto c = stack.top();

        std::vector<Dir> neighbours;

        for(auto d: AllDirs())
        {
            const auto np = c + DirToOffset(d);
            if(CanVisitWithoutMakingLoop(maze, np))
            {
                neighbours.push_back(d);
            }
        }

        if(neighbours.empty())
        {
            stack.pop();
        }
        else
        {
            const Dir dir = random->Next(neighbours);
            auto      np  = AddStepToMaze(maze, c, dir);
            stack.push(np);
            visited_cells += 1;
        }
    }


    //////////////////////////////////////////////////////////////////////////////////////////

    void
    AddToFrontier(
            Maze*                                maze,
            std::vector<RandomTraversal::Entry>* frontier,
            const vec2i&                         p)
    {
        Visit(maze, p);
        for(auto d: AllDirs())
        {
            if(CanVisitWithoutMakingLoop(maze, p + DirToOffset(d)))
            {
                frontier->push_back({p, d});
            }
        }
    }


    void
    RandomTraversal::Setup()
    {
        maze->Clear(Cell::None);

        AddToFrontier(maze, &frontier, RandomPositionOnMaze(random, maze));
    }


    bool
    RandomTraversal::HasMoreWork() const
    {
        return !frontier.empty();
    }


    void
    RandomTraversal::Work()
    {
        auto       f  = PopRandom(&frontier, random);
        const auto np = f.position + DirToOffset(f.direction);

        if(!CanVisitWithoutMakingLoop(maze, np))
        {
            return;
        }
        AddStepToMaze(maze, f.position, f.direction);

        AddToFrontier(maze, &frontier, np);
    }


    //////////////////////////////////////////////////////////////////////////////////////////

    Drawer::Drawer()
        : wall_color(Color::Black)
        , cell_color(Color::LightGray)
        , cell_visited_color(Color::White)
        , unit_color(Color::Red)
        , corridor_color(Color::White)
    {}

    rgbi
    Drawer::CalculateCellColor(int x, int y) const
    {
        const auto cell_value = (*maze)(x, y);

        if(tracker != nullptr && tracker->HasMoreWork() && !tracker->stack.empty())
        {
            const auto t = tracker->stack.top();
            if(x == t.x && y == t.y)
            {
                return unit_color;
            }
        }

        if(traversal != nullptr)
        {
            for(auto e: traversal->frontier)
            {
                auto p = e.position + DirToOffset(e.direction);
                if(p.x == x && p.y == y)
                {
                    return unit_color;
                }
            }
        }

        if((cell_value & Cell::Visited) != 0)
        {
            return cell_visited_color;
        }
        else
        {
            return cell_color;
        }
    }

    void
    Drawer::Draw()
    {
        const auto path_size = cell_size + wall_size;

        image.setup_no_alpha_support(
                wall_size + maze->GetWidth() * path_size,
                wall_size + maze->GetHeight() * path_size);

        Clear(&image, wall_color);

        for(int x = 0; x < maze->GetWidth(); x += 1)
        {
            for(int y = 0; y < maze->GetHeight(); y += 1)
            {
                const auto px = wall_size + x * path_size;
                const auto py = wall_size + y * path_size + cell_size - 1;

                DrawSquare(
                        &image,
                        CalculateCellColor(x, y),
                        px,
                        py,
                        cell_size);

                const auto xywh = [](int x, int y, int w, int h) {
                    return Recti::FromTopLeftWidthHeight(vec2i{x, y + 1}, w, h);
                };

                const auto cell_value = (*maze)(x, y);

                if((cell_value & Cell::PathSouth) != 0)
                {
                    DrawRect(
                            &image,
                            corridor_color,
                            xywh(px, py - cell_size, cell_size, wall_size));
                }
                if((cell_value & Cell::PathEast) != 0)
                {
                    DrawRect(
                            &image,
                            corridor_color,
                            xywh(px + cell_size, py, wall_size, cell_size));
                }
            }
        }
    }
}
