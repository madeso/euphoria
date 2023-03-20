    #include "core/table_bool.h"

#include "core/image_draw.h"
#include "assert/assert.h"
#include <cmath>

namespace euphoria::core
{
    void
    set_white_noise
    (
        BoolTable* world,
        Lrud<BorderSetupRule> border_control,
        std::function<bool()> rng
    )
    {
        world->set_all([&](int x, int y)
        {
            const auto is_on_left_border = x == 0;
            const auto is_on_right_border = x == world->get_width() - 1;
            const auto is_on_up_border = y == 0;
            const auto is_on_down_border = y == world->get_height() - 1;

#define CHECK_BORDER(b, p) \
            do \
            { \
                if( b && border_control.p != BorderSetupRule::random) \
                { return border_control.p == BorderSetupRule::always_wall; } \
            } while(false)

            CHECK_BORDER(is_on_left_border, left);
            CHECK_BORDER(is_on_right_border, right);
            CHECK_BORDER(is_on_up_border, up);
            CHECK_BORDER(is_on_down_border, down);
#undef CHECK_BORDER

            return rng();
        });
    }


    namespace
    {
        int
        count_single_walls
        (
            const BoolTable& world,
            Lrud<OutsideRule> outside_rule,
            int x,
            int y,
            int cx,
            int cy,
            bool include_self
        )
        {
            if (include_self==false && x == cx && y == cy)
            {
                // do not include self when counting walls
                return 0;
            }

            auto contains = [&world](int xx, int yy)
            {
                return world.get_indices().contains_inclusive(xx, yy);
            };

            if (contains(x, y))
            {
                // it is inside
                if (world(x, y))
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }

            // todo(Gustav): the x/y is too much copy/paste... fix!
            auto nx = x;
            if (!contains(x, 0))
            {
                // x is outside
                const auto xrule = x < 0 ? outside_rule.left : outside_rule.right;
                switch (xrule)
                {
                case OutsideRule::wall:
                    return 1;
                case OutsideRule::empty:
                    return 0;
                case OutsideRule::mirror:
                    // todo(Gustav): implement this!
                    nx = keep_within(world.get_indices().get_range_x(), x);
                    DIE("Implement this");
                    break;
                case OutsideRule::wrap:
                    nx = wrap(world.get_indices().get_range_x(), x);
                    break;
                default:
                    DIE("Unhandled case");
                    break;
                }
            }

            auto ny = y;
            if (!contains(0, y))
            {
                // y is outside
                const auto yrule = y < 0 ? outside_rule.up : outside_rule.down;
                switch (yrule)
                {
                case OutsideRule::wall:
                    return 1;
                case OutsideRule::empty:
                    // pass
                    return 0;
                case OutsideRule::mirror:
                    // todo(Gustav): implement this!
                    ny = keep_within(world.get_indices().get_range_y(), y);
                    DIE("Implement this");
                    break;
                case OutsideRule::wrap:
                    ny = wrap(world.get_indices().get_range_y(), y);
                    break;
                default:
                    DIE("Unhandled case");
                    break;
                }
            }

            if (world(nx, ny))
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }


    int
    count_walls_manhattan
    (
        const BoolTable& world,
        Lrud<OutsideRule> outside_rule,
        int cx,
        int cy,
        int step,
        bool include_self
    )
    {
        int walls = 0;

        for (int y = cy - step; y <= cy + step; y += 1)
        {
            for (int x = cx - step; x <= cx + step; x += 1)
            {
                const auto manhattan_distance = std::abs(x-cx) + std::abs(y-cy);
                if(manhattan_distance > step) { continue; }

                walls += count_single_walls
                (
                    world,
                    outside_rule,
                    x,
                    y,
                    cx,
                    cy,
                    include_self
                );
            }
        }

        return walls;
    }


    int
    count_walls_plus
    (
        const BoolTable& world,
        Lrud<OutsideRule> outside_rule,
        int cx,
        int cy,
        int step,
        bool include_self
    )
    {
        int walls = 0;

        auto calc_walls = [&](int x, int y)
        {
            walls += count_single_walls
            (
                world,
                outside_rule,
                x,
                y,
                cx,
                cy,
                include_self
            );
        };
        for (int y = cy - step; y <= cy + step; y += 1)
        {
            calc_walls(cx, y);
        }
        for (int x = cx - step; x <= cx + step; x += 1)
        {
            calc_walls(x, cy);
        }

        return walls;
    }


    int
    count_walls_box
    (
        const BoolTable& world,
        Lrud<OutsideRule> outside_rule,
        int cx,
        int cy,
        int step,
        bool include_self
    )
    {
        int walls = 0;

        for (int y = cy - step; y <= cy + step; y += 1)
        {
            for (int x = cx - step; x <= cx + step; x += 1)
            {
                walls += count_single_walls
                (
                    world,
                    outside_rule,
                    x,
                    y,
                    cx,
                    cy,
                    include_self
                );
            }
        }

        return walls;
    }


    WallCounter::WallCounter
    (
        const BoolTable& w,
        Lrud<core::OutsideRule> r,
        int x,
        int y
    )
        : world(w)
        , outside_rule(r)
        , cx(x)
        , cy(y)
    {
    }


    namespace
    {
        int
        count_walls
        (
            NeighborhoodAlgorithm algorithm,
            const BoolTable& world,
            Lrud<OutsideRule> outside_rule,
            int cx,
            int cy,
            int step,
            bool include_self
        )
        {
            switch(algorithm)
            {
                case NeighborhoodAlgorithm::manhattan:
                    return count_walls_manhattan
                        (world, outside_rule, cx, cy, step, include_self);
                case NeighborhoodAlgorithm::plus:
                    return count_walls_plus
                        (world, outside_rule, cx, cy, step, include_self);
                case NeighborhoodAlgorithm::box:
                    return count_walls_box
                        (world, outside_rule, cx, cy, step, include_self);

                default:
                    DIE("Unhandled algorithm");
                    return 0;
            }
        }
    }



    int
    WallCounter::count
    (
        int step,
        bool include_self,
        NeighborhoodAlgorithm algorithm
    ) const
    {
        return count_walls
        (
            algorithm,
            world,
            outside_rule,
            cx,
            cy,
            step,
            include_self
        );
    }


    void
    make_smoother
    (
        BoolTable* world,
        Lrud<OutsideRule> outside_rule,
        std::function<std::optional<bool>(bool, const WallCounter&)> smooth_function
    )
    {
        const BoolTable current = *world;

        world->set_all([&current, outside_rule, smooth_function](int x, int y)
        {
            const auto value = current(x, y);
            const auto walls = WallCounter{current, outside_rule, x, y};
            const auto smoothed_wall = smooth_function(value, walls);
            return smoothed_wall.value_or(value);
        });
    }


    std::vector<vec2i>
    find_empty_blocks(const BoolTable& world)
    {
        auto ret = std::vector<vec2i>{};

        for (int y = 0; y < world.get_height(); y += 1)
        {
            for (int x = 0; x < world.get_width(); x += 1)
            {
                if(world(x,y) == false)
                {
                    ret.emplace_back(x, y);
                }
            }
        }

        return ret;
    }

    std::vector<vec2i>
    find_flood_fill_items
    (
        const BoolTable& world,
        const vec2i& start,
        bool allow_diagonals
    )
    {
        auto visited = BoolTable::from_width_height
        (
            world.get_width(),
            world.get_height(),
            false
        );
        auto stack = std::vector<vec2i>{};
        auto ret = std::vector<vec2i>{};

        auto add_to_stack = [&](const vec2i& p)
        {
            if(p.x < 0) { return; }
            if(p.x >= world.get_width()) { return; }
            if(p.y < 0) { return; }
            if(p.y >= world.get_height()) { return; }
            if(visited(p.x, p.y) == true) { return; }
            visited(p.x, p.y) = true;
            // todo(Gustav): if ret contains p, return

            stack.emplace_back(p);
        };

        add_to_stack(start);
        while(stack.empty() == false)
        {
            const auto p = *stack.rbegin();
            stack.pop_back();
            if(world(p.x, p.y) == true) { continue; }

            // todo(Gustav): don't add if already in list
            ret.emplace_back(p);

            add_to_stack({p.x + 1, p.y});
            add_to_stack({p.x - 1, p.y});
            add_to_stack({p.x, p.y + 1});
            add_to_stack({p.x, p.y - 1});

            if(allow_diagonals)
            {
                add_to_stack({p.x + 1, p.y + 1});
                add_to_stack({p.x + 1, p.y - 1});
                add_to_stack({p.x - 1, p.y + 1});
                add_to_stack({p.x - 1, p.y - 1});
            }
        }

        return ret;
    }

    std::vector<std::vector<vec2i>>
    find_empty_regions(const BoolTable& world, bool allow_diagonals)
    {
        auto ret = std::vector<std::vector<vec2i>>{};

        auto visited = BoolTable::from_width_height
        (
            world.get_width(),
            world.get_height(),
            false
        );

        const auto blocks = find_empty_blocks(world);
        for(const auto block: blocks)
        {
            if(visited(block.x, block.y) == true) { continue; }

            const auto island = find_flood_fill_items(world, block, allow_diagonals);
            ret.emplace_back(island);
            for(const auto& island_block: island)
            {
                visited(island_block.x, island_block.y) = true;
            }
        }

        return ret;
    }



    BorderSettings::BorderSettings(const rgbai& c)
        : color(c)
    {
    }


    Image
    draw
    (
        const BoolTable& world,
        rgbai wall_color,
        rgbai space_color,
        int scale,
        std::optional<BorderSettings> border
    )
    {
        const bool has_alpha =
            wall_color.a < 255 ||
            space_color.a < 255 ||
            (border.has_value() ? border->color.a < 255 : false)
            ;

        const auto image_width = world.get_width() * scale;
        const auto image_height = world.get_height() * scale;

        Image image;
        if(has_alpha)
        {
            image.setup_with_alpha_support(image_width, image_height);
        }
        else
        {
            image.setup_no_alpha_support(image_width, image_height);
        }

        auto get_space_color = [&](int x, int y) -> rgbai
        {
            if(border.has_value() == false) { return space_color; }

            const auto rule = Lrud<OutsideRule>{OutsideRule::empty};
            const auto algorithm = NeighborhoodAlgorithm::plus;

            const auto wc = WallCounter{world, rule, x, y};
            const auto walls = wc.count(1, false, algorithm);

            if(walls>0) { return border->color; }
            else { return space_color; }
        };

        clear(&image, space_color);

        for (int y = 0; y < world.get_height(); y += 1)
        {
            for (int x = 0; x < world.get_width(); x += 1)
            {
                const auto px = x * scale;
                const auto py = y * scale;
                const auto color = world(x, y)
                    ? wall_color
                    : get_space_color(x, y)
                    ;
                draw_square(&image, color, px, py + scale - 1, scale);
            }
        }

        return image;
    }
}
