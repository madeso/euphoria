#include "core/generator_maze.h"

#include "core/random.h"
#include "core/image_draw.h"
#include "core/colors.h"

namespace euphoria::core::generator
{
    Vec2i
    dir_to_offset(const Direction d)
    {
        switch(d)
        {
        case Direction::south: return Vec2i { 0, -1};
        case Direction::north: return Vec2i { 0, 1};
        case Direction::west: return Vec2i {-1, 0};
        case Direction::east: return Vec2i { 1, 0};
        default: return Vec2i(0, 0);
        }
    }


    const std::vector<Direction>&
    all_dirs()
    {
        const static auto dirs = std::vector<Direction>
        {
            Direction::north, Direction::south, Direction::east, Direction::west
        };
        return dirs;
    }


    Direction
    flip_direction(const Direction d)
    {
        switch(d)
        {
        case Direction::north: return Direction::south;
        case Direction::south: return Direction::north;
        case Direction::east: return Direction::west;
        case Direction::west: return Direction::east;
        default: return Direction::north;
        }
    }


    cell::Type
    dir_to_cell_path(const Direction d)
    {
        switch(d)
        {
        case Direction::north: return cell::path_north;
        case Direction::south: return cell::path_south;
        case Direction::east: return cell::path_east;
        case Direction::west: return cell::path_west;
        default: return cell::path_north;
        }
    }


    void
    visit(Maze* maze, const Vec2i& np)
    {
        (*maze)(np.x, np.y) |= cell::visited;
    }


    Vec2i
    add_step_to_maze(Maze* maze, const Vec2i& c, Direction dir)
    {
        const auto o = dir_to_offset(dir);
        const auto np = c + o;
        (*maze)(np.x, np.y) |= cell::visited | dir_to_cell_path(flip_direction(dir));
        (*maze)(c.x, c.y) |= dir_to_cell_path(dir);
        return np;
    }


    bool
    has_visited(Maze* maze, const Vec2i& np)
    {
        return ((*maze)(np.x, np.y) & cell::visited) != 0;
    }


    bool
    can_visit_without_making_loop(Maze* maze, const Vec2i& np)
    {
        const auto world_size = Recti::from_width_height
        (
            maze->get_width() - 1,
            maze->get_height() - 1
        );
        return world_size.contains_inclusive(np) && !has_visited(maze, np);
    }


    template <typename T>
    T
    pop_random(std::vector<T>* vec, Random* r)
    {
        ASSERT(!vec->empty());
        const auto i = get_random_in_range(r, vec->size());
        T t = (*vec)[i];
        vec->erase(vec->begin() + i);
        return t;
    }


    Vec2i
    random_position_on_maze(Random* random, Maze* maze)
    {
        return
        {
            get_random_in_range(random, maze->get_width()),
            get_random_in_range(random, maze->get_height())
        };
    }


    //////////////////////////////////////////////////////////////////////////////////////////

    void
    RecursiveBacktracker::setup()
    {
        maze->clear(cell::none);

        const auto random_position = random_position_on_maze(random, maze);

        stack.push(random_position);
        (*maze)(random_position.x, random_position.y) = cell::visited;
        visited_cells = 1;
    }


    bool
    RecursiveBacktracker::has_more_work() const
    {
        return visited_cells < maze->get_width() * maze->get_height();
    }


    void
    RecursiveBacktracker::work()
    {
        const auto c = stack.top();

        std::vector<Direction> neighbours;

        for(auto d: all_dirs())
        {
            const auto np = c + dir_to_offset(d);
            if(can_visit_without_making_loop(maze, np))
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
            const Direction dir = get_random_item_in_vector(random, neighbours);
            auto np = add_step_to_maze(maze, c, dir);
            stack.push(np);
            visited_cells += 1;
        }
    }


    //////////////////////////////////////////////////////////////////////////////////////////

    void
    add_to_frontier
    (
        Maze* maze,
        std::vector<RandomTraversal::Entry>* frontier,
        const Vec2i& p
    )
    {
        visit(maze, p);
        for(auto d: all_dirs())
        {
            if(can_visit_without_making_loop(maze, p + dir_to_offset(d)))
            {
                frontier->push_back({p, d});
            }
        }
    }


    void
    RandomTraversal::setup()
    {
        maze->clear(cell::none);
        add_to_frontier(maze, &frontier, random_position_on_maze(random, maze));
    }


    bool
    RandomTraversal::has_more_work() const
    {
        return !frontier.empty();
    }


    void
    RandomTraversal::work()
    {
        auto f = pop_random(&frontier, random);
        const auto np = f.position + dir_to_offset(f.direction);

        if(!can_visit_without_making_loop(maze, np))
        {
            return;
        }

        add_step_to_maze(maze, f.position, f.direction);
        add_to_frontier(maze, &frontier, np);
    }


    //////////////////////////////////////////////////////////////////////////////////////////


    Drawer::Drawer()
        : wall_color(NamedColor::black)
        , cell_color(NamedColor::light_gray)
        , cell_visited_color(NamedColor::white)
        , unit_color(NamedColor::red)
        , corridor_color(NamedColor::white)
    {
    }


    Rgbi
    Drawer::calculate_cell_color(int x, int y) const
    {
        const auto cell_value = (*maze)(x, y);

        if(tracker != nullptr && tracker->has_more_work() && !tracker->stack.empty())
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
                auto p = e.position + dir_to_offset(e.direction);
                if(p.x == x && p.y == y)
                {
                    return unit_color;
                }
            }
        }

        if((cell_value & cell::visited) != 0)
        {
            return cell_visited_color;
        }
        else
        {
            return cell_color;
        }
    }


    void
    Drawer::draw()
    {
        const auto path_size = cell_size + wall_size;

        image.setup_no_alpha_support
        (
            wall_size + maze->get_width() * path_size,
            wall_size + maze->get_height() * path_size
        );

        clear(&image, wall_color);

        for(int x = 0; x < maze->get_width(); x += 1)
        {
            for(int y = 0; y < maze->get_height(); y += 1)
            {
                const auto px = wall_size + x * path_size;
                const auto py = wall_size + y * path_size + cell_size - 1;

                draw_square
                (
                    &image,
                    calculate_cell_color(x, y),
                    px,
                    py,
                    cell_size
                );

                const auto xywh = [](int ax, int ay, int aw, int ah)
                {
                    return Recti::from_top_left_width_height(Vec2i{ax, ay + 1}, aw, ah);
                };

                const auto cell_value = (*maze)(x, y);

                if((cell_value & cell::path_south) != 0)
                {
                    draw_rect
                    (
                        &image,
                        corridor_color,
                        xywh(px, py - cell_size, cell_size, wall_size)
                    );
                }
                if((cell_value & cell::path_east) != 0)
                {
                    draw_rect
                    (
                        &image,
                        corridor_color,
                        xywh(px + cell_size, py, wall_size, cell_size)
                    );
                }
            }
        }
    }


}
