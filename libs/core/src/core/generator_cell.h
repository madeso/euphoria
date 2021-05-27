#ifndef CORE_GENERATOR_CELL_H
#define CORE_GENERATOR_CELL_H

#include "core/fourway.h"
#include "core/table_bool.h"

#include <vector>

namespace euphoria::core
{
    struct random;
}

namespace euphoria::core::generator
{
    using world = bool_table;

    // make generation better
    // http://www.roguebasin.com/index.php?title=Cellular_Automata_Method_for_Generating_Random_Cave-Like_Levels

    struct cellular_automata;

    struct rule
    {
        rule() = default;
        virtual ~rule() = default;

        rule(const rule&) = delete;
        rule(rule&&) = delete;
        void operator=(const rule&) = delete;
        void operator=(rule&&) = delete;

        virtual void step(cellular_automata* self) = 0;
    };


    struct rules
    {
        std::vector<std::shared_ptr<rule>> rules;

        void
        add_rule(int count, std::shared_ptr<rule> rule);
    };

    struct cellular_automata
    {
        generator::rules* rules;
        generator::world* world;
        fourway<core::outside_rule> outside_rule;
        int iteration;

        cellular_automata(generator::rules* r, generator::world* w, const fourway<core::outside_rule>& fw);

        [[nodiscard]] bool
        has_more_work() const;

        void
        work();
    };

    using change_function = std::function
    <
        std::optional<bool>
        (bool, const wallcounter&)
    >;

    // todo(Gustav): rename rules to make it clearer what they do

    /// fills the world with random data
    void
    add_random_fill
    (
        rules* cell,
        random* random,
        float random_fill = 0.5,
        fourway<border_setup_rule> border_control = fourway<border_setup_rule>
        {
            border_setup_rule::always_wall
        }
    );

    /// 'clears' cells with less than 'count' neighbours, good for removing 'blobs'
    void
    add_clear_rules
    (
        rules* ca,
        int times,
        int count,
        int range,
        bool include_self,
        neighborhood_algorithm algorithm
    );

    void
    add_complex_rules
    (
        rules* ca,
        int times,
        change_function change
    );

    /// simple smoothing, less than count neighbours are removed, more -> solid
    void
    add_simple_rules
    (
        rules* ca,
        int times,
        int count,
        bool include_self,
        neighborhood_algorithm algorithm
    );

    /// blanks out cells at 'y' and 'height' down
    void
    add_horizontal_blank_rule
    (
        rules* ca,
        int y,
        int height
    );

    /// simple smoothing, but always set the cell, seems to provide spikes
    void
    add_spiky_rules
    (
        rules* ca,
        int times,
        int count,
        bool include_self,
        neighborhood_algorithm algorithm
    );

    /// smooth but 'big_count' is applied for R(2)
    void
    add_combo_rules
    (
        rules* ca,
        int times,
        int count,
        int big_count,
        bool include_self,
        neighborhood_algorithm algorithm
    );

    /// fills all open areas that are less than 'min_count'
    void
    add_fill_small_holes_rule(rules* rules, bool allow_diagonals, int min_count);

    /// fills all open areas, but keeps 'holes_to_keep'
    void
    add_fill_all_holes_rule(rules* rules, bool allow_diagonals, int holes_to_keep);
}

#endif // CORE_GENERATOR_CELL_H
