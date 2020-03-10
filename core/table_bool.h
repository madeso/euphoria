#ifndef CORE_STRING_TABLE_H
#define CORE_STRING_TABLE_H

#include "core/table.h"

#include "core/rgb.h"
#include "core/image.h"
#include "core/fourway.h"
#include "core/enumtostring.h"

#include <functional>
#include <optional>

namespace euphoria::core
{
    using BoolTable = Table<bool>;

    ///////////////////////////////////////////////////////////////////////////// ========================================================================
    // Setup

    enum class BorderSetupRule
    {
        AlwaysWall,
        AlwaysEmpty,
        Random,
    };

    void
    SetWhiteNoise
    (
        BoolTable* world,
        Fourway<BorderSetupRule> border_control,
        std::function<bool()> rng
    );

    ///////////////////////////////////////////////////////////////////////////
    // Smooth step

    enum class OutsideRule
    {
        Wall, Empty, Mirror, Wrap
    };

    int
    CountWalls
    (
        const BoolTable& world,
        Fourway<OutsideRule> outside_rule,
        int cx,
        int cy,
        int step
    );


    struct Wallcounter
    {
        const BoolTable& world;
        Fourway<OutsideRule> outside_rule;
        int cx;
        int cy;

        Wallcounter
        (
            const BoolTable& w,
            Fourway<OutsideRule> r,
            int x,
            int y
        );

        int
        Count(int step) const;
    };


    void
    SmoothMap
    (
        BoolTable* world,
        Fourway<OutsideRule> outside_rule,
        // return if occupied or not, or nullopt to keep old value
        std::function<std::optional<bool>(const Wallcounter&)> smooth_function
    );

    ///////////////////////////////////////////////////////////////////////////
    // Rendering

    Image
    Draw
    (
        const BoolTable& world,
        Rgbi wall_color,
        Rgbi space_color,
        int scale
    );

} 

DECLARE_ENUM_LIST(euphoria::core::BorderSetupRule)
DECLARE_ENUM_LIST(euphoria::core::OutsideRule)

#endif  // CORE_STRING_TABLE_H
