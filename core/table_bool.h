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

    /* Count walls according to von Neumann neighborhood 'manhattan distance' rule.
    */
    int
    CountWallsManhattan
    (
        const BoolTable& world,
        Fourway<OutsideRule> outside_rule,
        int cx,
        int cy,
        int step,
        bool include_self
    );


    /* Count walls according to extended von Neumann neighborhood 'plus' rule.
    */
    int
    CountWallsPlus
    (
        const BoolTable& world,
        Fourway<OutsideRule> outside_rule,
        int cx,
        int cy,
        int step,
        bool include_self
    );


    /* Count walls according to Moore neighborhood 'box' rule.
    */
    int
    CountWallsBox
    (
        const BoolTable& world,
        Fourway<OutsideRule> outside_rule,
        int cx,
        int cy,
        int step,
        bool include_self
    );

    enum class NeighborhoodAlgorithm
    {
        // von Neumann
        Manhattan,

        // extended vonNeumann
        Plus,

        // Moore
        Box
    };


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
        Count(int step, bool include_self, NeighborhoodAlgorithm algorithm) const;
    };


    void
    SmoothMap
    (
        BoolTable* world,
        Fourway<OutsideRule> outside_rule,
        // return if occupied or not, or nullopt to keep old value
        std::function<std::optional<bool>(bool, const Wallcounter&)> smooth_function
    );


    ///////////////////////////////////////////////////////////////////////////
    // FloodFill

    std::vector<vec2i>
    FindEmptyBlocks(const BoolTable& world);

    std::vector<vec2i>
    FindFloodFillItems
    (
        const BoolTable& world,
        const vec2i& start,
        bool allow_diagonals
    );

    std::vector<std::vector<vec2i>>
    FindEmptyRegions(const BoolTable& world, bool allow_diagonals);

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

#endif  // CORE_STRING_TABLE_H
