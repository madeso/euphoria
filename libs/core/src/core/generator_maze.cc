#include "core/generator_maze.h"

#include "core/random.h"
#include "core/image_draw.h"
#include "core/colors.h"

namespace euphoria::core::generator
{
    vec2i
    dir_to_offset(const dir d)
    {
        switch(d)
        {
        case dir::south: return vec2i { 0, -1};
        case dir::north: return vec2i { 0,  1};
        case dir::west:  return vec2i {-1,  0};
        case dir::east:  return vec2i { 1,  0};
        default: return vec2i(0, 0);
        }
    }


    const std::vector<dir>&
    all_dirs()
    {
        const static auto dirs = std::vector<dir> {
                dir::north, dir::south, dir::east, dir::west};
        return dirs;
    }


    dir
    flip_direction(const dir d)
    {
        switch(d)
        {
        case dir::north: return dir::south;
        case dir::south: return dir::north;
        case dir::east: return dir::west;
        case dir::west: return dir::east;
        default: return dir::north;
        }
    }


    cell::type
    dir_to_cell_path(const dir d)
    {
        switch(d)
        {
        case dir::north: return cell::path_north;
        case dir::south: return cell::path_south;
        case dir::east: return cell::path_east;
        case dir::west: return cell::path_west;
        default: return cell::path_north;
        }
    }


    void
    visit(maze* maze, const vec2i& np)
    {
        (*maze)(np.x, np.y) |= cell::visited;
    }


    vec2i
    add_step_to_maze(maze* maze, const vec2i& c, dir dir)
    {
        const auto o  = dir_to_offset(dir);
        const auto np = c + o;
        (*maze)(np.x, np.y)
                |= cell::visited | dir_to_cell_path(flip_direction(dir));
        (*maze)(c.x, c.y) |= dir_to_cell_path(dir);
        return np;
    }


    bool
    has_visited(maze* maze, const vec2i& np)
    {
        return ((*maze)(np.x, np.y) & cell::visited) != 0;
    }


    bool
    can_visit_without_making_loop(maze* maze, const vec2i& np)
    {
        const auto world_size = Recti::FromWidthHeight(
                maze->get_width() - 1, maze->get_height() - 1);
        return world_size.ContainsInclusive(np) && !has_visited(maze, np);
    }


    template <typename T>
    T
    pop_random(std::vector<T>* vec, Random* r)
    {
        ASSERT(!vec->empty());
        const auto i = r->NextRange(vec->size());
        T          t = (*vec)[i];
        vec->erase(vec->begin() + i);
        return t;
    }


    vec2i
    random_position_on_maze(Random* random, maze* maze)
    {
        return {random->NextRange(maze->get_width()),
                random->NextRange(maze->get_height())};
    }


    //////////////////////////////////////////////////////////////////////////////////////////

    void
    recursive_backtracker::setup()
    {
        maze->clear(cell::none);

        const auto random_position = random_position_on_maze(random, maze);

        stack.push(random_position);
        (*maze)(random_position.x, random_position.y) = cell::visited;
        visited_cells = 1;
    }


    bool
    recursive_backtracker::has_more_work() const
    {
        return visited_cells < maze->get_width() * maze->get_height();
    }


    void
    recursive_backtracker::work()
    {
        const auto c = stack.top();

        std::vector<dir> neighbours;

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
            const dir dir = random->Next(neighbours);
            auto      np  = add_step_to_maze(maze, c, dir);
            stack.push(np);
            visited_cells += 1;
        }
    }


    //////////////////////////////////////////////////////////////////////////////////////////

    void
    add_to_frontier(
            maze*                                maze,
            std::vector<random_traversal::entry>* frontier,
            const vec2i&                         p)
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
    random_traversal::setup()
    {
        maze->clear(cell::none);

        add_to_frontier(maze, &frontier, random_position_on_maze(random, maze));
    }


    bool
    random_traversal::has_more_work() const
    {
        return !frontier.empty();
    }


    void
    random_traversal::work()
    {
        auto       f  = pop_random(&frontier, random);
        const auto np = f.position + dir_to_offset(f.direction);

        if(!can_visit_without_making_loop(maze, np))
        {
            return;
        }
        add_step_to_maze(maze, f.position, f.direction);

        add_to_frontier(maze, &frontier, np);
    }


    //////////////////////////////////////////////////////////////////////////////////////////

    drawer::drawer()
        : wall_color(color::black)
        , cell_color(color::light_gray)
        , cell_visited_color(color::white)
        , unit_color(color::red)
        , corridor_color(color::white)
    {}

    rgbi
    drawer::calculate_cell_color(int x, int y) const
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
    drawer::draw()
    {
        const auto path_size = cell_size + wall_size;

        image.setup_no_alpha_support(
                wall_size + maze->get_width() * path_size,
                wall_size + maze->get_height() * path_size);

        clear(&image, wall_color);

        for(int x = 0; x < maze->get_width(); x += 1)
        {
            for(int y = 0; y < maze->get_height(); y += 1)
            {
                const auto px = wall_size + x * path_size;
                const auto py = wall_size + y * path_size + cell_size - 1;

                draw_square(
                        &image,
                        calculate_cell_color(x, y),
                        px,
                        py,
                        cell_size);

                const auto xywh = [](int x, int y, int w, int h) {
                    return Recti::FromTopLeftWidthHeight(vec2i{x, y + 1}, w, h);
                };

                const auto cell_value = (*maze)(x, y);

                if((cell_value & cell::path_south) != 0)
                {
                    draw_rect(
                            &image,
                            corridor_color,
                            xywh(px, py - cell_size, cell_size, wall_size));
                }
                if((cell_value & cell::path_east) != 0)
                {
                    draw_rect(
                            &image,
                            corridor_color,
                            xywh(px + cell_size, py, wall_size, cell_size));
                }
            }
        }
    }
}
