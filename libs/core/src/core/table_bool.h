#pragma once

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
        always_wall,
        always_empty,
        random,
    };

    void
    set_white_noise
    (
        BoolTable* world,
        Lrud<BorderSetupRule> border_control,
        std::function<bool()> rng
    );

    ///////////////////////////////////////////////////////////////////////////
    // Smooth step

    enum class OutsideRule
    {
        wall, empty, mirror, wrap
    };

    /* Count walls according to von Neumann neighborhood 'manhattan distance' rule.
    */
    int
    count_walls_manhattan
    (
        const BoolTable& world,
        Lrud<OutsideRule> outside_rule,
        int cx,
        int cy,
        int step,
        bool include_self
    );


    /* Count walls according to extended von Neumann neighborhood 'plus' rule.
    */
    int
    count_walls_plus
    (
        const BoolTable& world,
        Lrud<OutsideRule> outside_rule,
        int cx,
        int cy,
        int step,
        bool include_self
    );


    /* Count walls according to Moore neighborhood 'box' rule.
    */
    int
    count_walls_box
    (
        const BoolTable& world,
        Lrud<OutsideRule> outside_rule,
        int cx,
        int cy,
        int step,
        bool include_self
    );

    enum class NeighborhoodAlgorithm
    {
        // von Neumann
        manhattan,

        // extended vonNeumann
        plus,

        // Moore
        box
    };


    struct WallCounter
    {
        const BoolTable& world;
        Lrud<core::OutsideRule> outside_rule;
        int cx;
        int cy;

        WallCounter
        (
            const BoolTable& w,
            Lrud<core::OutsideRule> r,
            int x,
            int y
        );

        [[nodiscard]] int
        count(int step, bool include_self, NeighborhoodAlgorithm algorithm) const;
    };


    void
    smooth_map
    (
        BoolTable* world,
        Lrud<OutsideRule> outside_rule,
        // return if occupied or not, or nullopt to keep old value
        std::function<std::optional<bool>(bool, const WallCounter&)> smooth_function
    );


    ///////////////////////////////////////////////////////////////////////////
    // FloodFill

    std::vector<vec2i>
    find_empty_blocks(const BoolTable& world);

    std::vector<vec2i>
    find_flood_fill_items
    (
        const BoolTable& world,
        const vec2i& start,
        bool allow_diagonals
    );

    std::vector<std::vector<vec2i>>
    find_empty_regions(const BoolTable& world, bool allow_diagonals);

    ///////////////////////////////////////////////////////////////////////////
    // Rendering

    struct BorderSettings
    {
        explicit BorderSettings(const rgbai& c);

        rgbai color;
    };

    Image
    draw
    (
        const BoolTable& world,
        rgbai wall_color,
        rgbai space_color,
        int scale,
        // if nullopt, no border
        std::optional<BorderSettings> border
    );
}
