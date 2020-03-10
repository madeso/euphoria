#ifndef CORE_GENERATOR_CELL_H
#define CORE_GENERATOR_CELL_H

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
    using World = Table<bool>;

    enum class BorderControl
    {
        AlwaysWall,
        AlwaysEmpty,
        RandomWall,
        RandomEmpty
    };

    // make generation better
    // http://www.roguebasin.com/index.php?title=Cellular_Automata_Method_for_Generating_Random_Cave-Like_Levels

    struct CellularAutomata
    {
        World*  world  = nullptr;
        Random* random = nullptr;

        float         random_fill    = 0.5;
        BorderControl border_control = BorderControl::AlwaysWall;

        int iteration = 0;

        void
        Setup();

        bool
        HasMoreWork() const;

        void
        Work();
    };

    Image
    Draw
    (
        const World& world,
        Rgbi wall_color,
        Rgbi space_color,
        int scale = 1
    );
}

DECLARE_ENUM_LIST(euphoria::core::generator::BorderControl)

#endif  // CORE_GENERATOR_CELL_H
