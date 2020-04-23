#include "core/table_bool.h"

#include "core/image_draw.h"
#include "core/assert.h"

namespace euphoria::core
{
    void
    SetWhiteNoise
    (
        BoolTable* world,
        Fourway<BorderSetupRule> border_control,
        std::function<bool()> rng
    )
    {
        world->SetAll([&](int x, int y)
        {
            const auto is_on_left_border = x == 0;
            const auto is_on_right_border = x == world->GetWidth() - 1;
            const auto is_on_up_border = y == 0;
            const auto is_on_down_border = y == world->GetHeight() - 1;

#define CHECK_BORDER(b, p) \
            do \
            { \
                if( b && border_control.p != BorderSetupRule::Random ) \
                { return border_control.p == BorderSetupRule::AlwaysWall; } \
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
        CountSingleWall
        (
            const BoolTable& world,
            Fourway<OutsideRule> outside_rule,
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

            auto contains = [&world](int x, int y)
            {
                return world.Indices().ContainsInclusive(x, y);
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
                case OutsideRule::Wall:
                    return 1;
                case OutsideRule::Empty:
                    return 0;
                case OutsideRule::Mirror:
                    // todo(Gustav): implement this!
                    nx = KeepWithin(world.Indices().GetXRange(), x);
                    DIE("Implement this");
                    break;
                case OutsideRule::Wrap:
                    nx = Wrap(world.Indices().GetXRange(), x);
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
                case OutsideRule::Wall:
                    return 1;
                case OutsideRule::Empty:
                    // pass
                    return 0;
                case OutsideRule::Mirror:
                    // todo(Gustav): implement this!
                    ny = KeepWithin(world.Indices().GetYRange(), y);
                    DIE("Implement this");
                    break;
                case OutsideRule::Wrap:
                    ny = Wrap(world.Indices().GetYRange(), y);
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
    CountWallsManhattan
    (
        const BoolTable& world,
        Fourway<OutsideRule> outside_rule,
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
                const auto manhattan_distance = Abs(x-cx) + Abs(y-cy);
                if(manhattan_distance > step) { continue; }

                walls += CountSingleWall
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
    CountWallsPlus
    (
        const BoolTable& world,
        Fourway<OutsideRule> outside_rule,
        int cx,
        int cy,
        int step,
        bool include_self
    )
    {
        int walls = 0;

        auto calc_walls = [&](int x, int y)
        {
            walls += CountSingleWall
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
    CountWallsBox
    (
        const BoolTable& world,
        Fourway<OutsideRule> outside_rule,
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
                walls += CountSingleWall
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


    Wallcounter::Wallcounter
    (
        const BoolTable& w,
        Fourway<OutsideRule> r,
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
        CountWalls
        (
            NeighborhoodAlgorithm algorithm,
            const BoolTable& world,
            Fourway<OutsideRule> outside_rule,
            int cx,
            int cy,
            int step,
            bool include_self
        )
        {
            switch(algorithm)
            {
                case NeighborhoodAlgorithm::Manhattan:
                    return CountWallsManhattan
                        (world, outside_rule, cx, cy, step, include_self);
                case NeighborhoodAlgorithm::Plus:
                    return CountWallsPlus
                        (world, outside_rule, cx, cy, step, include_self);
                case NeighborhoodAlgorithm::Box:
                    return CountWallsBox
                        (world, outside_rule, cx, cy, step, include_self);

                default:
                    DIE("Unhandled algorithm"); 
                    return 0;
            }
        }
    }



    int
    Wallcounter::Count
    (
        int step,
        bool include_self,
        NeighborhoodAlgorithm algorithm
    ) const
    {
        return CountWalls
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
    SmoothMap
    (
        BoolTable* world,
        Fourway<OutsideRule> outside_rule,
        std::function<std::optional<bool>(bool, const Wallcounter&)> smooth_function
    )
    {
        const BoolTable current = *world;

        world->SetAll([&current, outside_rule, smooth_function](int x, int y)
        {
            const auto value = current(x, y);
            const auto walls = Wallcounter{current, outside_rule, x, y};
            const auto smoothed_wall = smooth_function(value, walls);
            return smoothed_wall.value_or(value);
        });
    }


    std::vector<vec2i>
    FindEmptyBlocks(const BoolTable& world)
    {
        auto ret = std::vector<vec2i>{};

        for (int y = 0; y < world.GetHeight(); y += 1)
        for (int x = 0; x < world.GetWidth(); x += 1)
        {
            if(world(x,y) == false)
            {
                ret.emplace_back(x, y);
            }
        }

        return ret;
    }

    std::vector<vec2i>
    FindFloodFillItems
    (
        const BoolTable& world,
        const vec2i& start,
        bool allow_diagonals
    )
    {
        auto visited = BoolTable::FromWidthHeight
        (
            world.GetWidth(),
            world.GetHeight(),
            false
        );
        auto stack = std::vector<vec2i>{};
        auto ret = std::vector<vec2i>{};

        auto add_to_stack = [&](const vec2i& p)
        {
            if(p.x < 0) { return; }
            if(p.x >= world.GetWidth()) { return; }
            if(p.y < 0) { return; }
            if(p.y >= world.GetHeight()) { return; }
            if(visited(p.x, p.y) == true) return;
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
    FindEmptyRegions(const BoolTable& world, bool allow_diagonals)
    {
        auto ret = std::vector<std::vector<vec2i>>{};

        auto visited = BoolTable::FromWidthHeight
        (
            world.GetWidth(),
            world.GetHeight(),
            false
        );

        const auto blocks = FindEmptyBlocks(world);
        for(const auto block: blocks)
        {
            if(visited(block.x, block.y) == true) { continue; }

            const auto island = FindFloodFillItems(world, block, allow_diagonals);
            ret.emplace_back(island);
            for(const auto& island_block: island)
            {
                visited(island_block.x, island_block.y) = true;
            }
        }

        return ret;
    }



    BorderSettings::BorderSettings(const Rgbi& c)
        : color(c)
    {
    }


    Image
    Draw
    (
        const BoolTable& world,
        Rgbi wall_color,
        Rgbi space_color,
        int scale,
        std::optional<BorderSettings> border
    )
    {
        Image image;
        image.SetupNoAlphaSupport
        (
            world.GetWidth() * scale,
            world.GetHeight() * scale
        );

        auto get_space_color = [&](int x, int y) -> Rgbi
        {
            if(border.has_value() == false) { return space_color; }

            const auto rule = Fourway<OutsideRule>{OutsideRule::Empty};
            const auto algorithm = NeighborhoodAlgorithm::Plus;

            const auto wc = Wallcounter{world, rule, x, y};
            const auto walls = wc.Count(1, false, algorithm);

            if(walls>0) { return border->color; }
            else { return space_color; }
        };

        Clear(&image, space_color);

        for (int y = 0; y < world.GetHeight(); y += 1)
        for (int x = 0; x < world.GetWidth(); x += 1)
        {
            const auto px = x * scale;
            const auto py = y * scale;
            const auto color = world(x, y)
                ? wall_color
                : get_space_color(x, y)
                ;
            DrawSquare(&image, color, px, py + scale - 1, scale);
        }

        return image;
    }
}
