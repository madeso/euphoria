#include "core/generator_cell.h"

#include "core/random.h"
#include "core/image_draw.h"
#include "core/colors.h"
#include "core/image.h"

namespace euphoria::core
{
    namespace generator
    {
        void
        CellularAutomata::Setup()
        {
            world->SetAll([this](int x, int y)
            {
                if
                (
                    border_control != BorderControl::RandomWall &&
                    border_control != BorderControl::RandomEmpty
                )
                {
                    const auto is_border =
                        x == 0 ||
                        y == 0 ||
                        x == world->GetWidth() - 1 ||
                        y == world->GetHeight() - 1;

                    if(is_border)
                    {
                        return border_control == BorderControl::AlwaysWall;
                    }
                }
                return random->NextFloat01() < random_fill;
            });
        }


        bool
        CellularAutomata::HasMoreWork() const
        {
            return iteration < 5;
        }

        int
        CountWalls
        (
            const World& world,
            bool outside_is_wall,
            int cx,
            int cy,
            int step = 1
        )
        {
            int walls = 0;
            for(int y = cy - step; y <= cy + step; y += 1)
            {
                for(int x = cx - step; x <= cx + step; x += 1)
                {
                    if(x == cx && y == cy)
                        continue;
                    if(!world.Indices().ContainsInclusive(x, y))
                    {
                        if(outside_is_wall)
                        {
                            walls += 1;
                        }
                    }
                    else
                    {
                        if (world(x, y))
                        {
                            walls += 1;
                        }
                    }
                }
            }
            return walls;
        }

        void
        SmoothMap(World* world, bool outside_is_wall)
        {
            const World current = *world;

            world->SetAll([&current, outside_is_wall](int x, int y)
            {
                const auto walls = CountWalls(current, outside_is_wall, x, y);
                if(walls > 4)
                {
                    return true;
                }
                if(walls < 4)
                {
                    return false;
                }
                return current(x, y);
            });
        }

        void
        CellularAutomata::Work()
        {
            const auto outside_is_wall =
                border_control == BorderControl::AlwaysWall ||
                border_control == BorderControl::RandomWall ;
            SmoothMap
            (
                world,
                outside_is_wall
            );

            iteration += 1;
        }

        CellularAutomataDrawer::CellularAutomataDrawer()
            : world(nullptr)
            , wall_color(Color::Black)
            , space_color(Color::White)
        {}

        void
        CellularAutomataDrawer::Draw()
        {
            image.SetupNoAlphaSupport(
                    world->GetWidth() * scale, world->GetHeight() * scale);

            Clear(&image, space_color);

            for(int x = 0; x < world->GetWidth(); x += 1)
            {
                for(int y = 0; y < world->GetHeight(); y += 1)
                {
                    const auto px = x * scale;
                    const auto py = y * scale;
                    const auto color = (*world)(x, y) ? wall_color : space_color;
                    DrawSquare(&image, color, px, py + scale - 1, scale);
                }
            }
        }
    }

    BEGIN_ENUM_LIST(generator::BorderControl)
    ENUM_VALUE(generator::BorderControl, AlwaysWall)
    ENUM_VALUE(generator::BorderControl, AlwaysEmpty)
    ENUM_VALUE(generator::BorderControl, RandomWall)
    ENUM_VALUE(generator::BorderControl, RandomEmpty)
    END_ENUM_LIST()

}  // namespace euphoria::core
