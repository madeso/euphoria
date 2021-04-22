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
    using bool_table = table<bool>;

    ///////////////////////////////////////////////////////////////////////////// ========================================================================
    // Setup

    enum class border_setup_rule
    {
        always_wall,
        always_empty,
        random,
    };

    void
    set_white_noise
    (
        bool_table* world,
        fourway<border_setup_rule> border_control,
        std::function<bool()> rng
    );

    ///////////////////////////////////////////////////////////////////////////
    // Smooth step

    enum class outside_rule
    {
        wall, empty, mirror, wrap
    };

    /* Count walls according to von Neumann neighborhood 'manhattan distance' rule.
    */
    int
    count_walls_manhattan
    (
        const bool_table& world,
        fourway<outside_rule> outside_rule,
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
        const bool_table& world,
        fourway<outside_rule> outside_rule,
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
        const bool_table& world,
        fourway<outside_rule> outside_rule,
        int cx,
        int cy,
        int step,
        bool include_self
    );

    enum class neighborhood_algorithm
    {
        // von Neumann
        manhattan,

        // extended vonNeumann
        plus,

        // Moore
        box
    };


    struct wallcounter
    {
        const bool_table& world;
        fourway<core::outside_rule> outside_rule;
        int cx;
        int cy;

        wallcounter
        (
            const bool_table& w,
            fourway<core::outside_rule> r,
            int x,
            int y
        );

        [[nodiscard]] int
        count(int step, bool include_self, neighborhood_algorithm algorithm) const;
    };


    void
    smooth_map
    (
        bool_table* world,
        fourway<outside_rule> outside_rule,
        // return if occupied or not, or nullopt to keep old value
        std::function<std::optional<bool>(bool, const wallcounter&)> smooth_function
    );


    ///////////////////////////////////////////////////////////////////////////
    // FloodFill

    std::vector<vec2i>
    find_empty_blocks(const bool_table& world);

    std::vector<vec2i>
    find_flood_fill_items
    (
        const bool_table& world,
        const vec2i& start,
        bool allow_diagonals
    );

    std::vector<std::vector<vec2i>>
    find_empty_regions(const bool_table& world, bool allow_diagonals);

    ///////////////////////////////////////////////////////////////////////////
    // Rendering

    struct border_settings
    {
        explicit border_settings(const rgbai& c);

        rgbai color;
    };

    image
    draw
    (
        const bool_table& world,
        rgbai wall_color,
        rgbai space_color,
        int scale,
        // if nullopt, no border
        std::optional<border_settings> border
    );
}
