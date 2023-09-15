#pragma once

#include "core/fourway.h"
#include "core/table_bool.h"



namespace eu::core
{
    struct Random;
}

namespace eu::core::generator
{
    using World = BoolTable;

    // make generation better
    // http://www.roguebasin.com/index.php?title=Cellular_Automata_Method_for_Generating_Random_Cave-Like_Levels

    struct CellularAutomata;

    struct Rule
    {
        Rule() = default;
        virtual ~Rule() = default;

        Rule(const Rule&) = delete;
        Rule(Rule&&) = delete;
        void operator=(const Rule&) = delete;
        void operator=(Rule&&) = delete;

        virtual void update(CellularAutomata* self) = 0;
    };


    struct Rules
    {
        std::vector<std::shared_ptr<Rule>> rules;

        void add_rule(int count, std::shared_ptr<Rule> rule);
    };

    struct CellularAutomata
    {
        generator::Rules* rules;
        generator::World* world;
        Lrud<core::OutsideRule> outside_rule;
        int iteration;

        CellularAutomata(generator::Rules* r, generator::World* w, const Lrud<core::OutsideRule>& fw);

        void update();

        [[nodiscard]] bool is_done() const;
    };

    using ChangeFunction = std::function
    <
        std::optional<bool>
        (bool, const WallCounter&)
    >;

    // todo(Gustav): rename rules to make it clearer what they do

    /// fills the world with random data
    void
    add_random_fill
    (
        Rules* cell,
        Random* random,
        float random_fill = 0.5,
        Lrud<BorderSetupRule> border_control = Lrud<BorderSetupRule>
        {
            BorderSetupRule::always_wall
        }
    );

    /// 'clears' cells with less than 'count' neighbours, good for removing 'blobs'
    void
    add_clear_rules
    (
        Rules* ca,
        int times,
        int count,
        int range,
        bool include_self,
        NeighborhoodAlgorithm algorithm
    );

    void
    add_complex_rules
    (
        Rules* ca,
        int times,
        ChangeFunction change
    );

    /// simple smoothing, less than count neighbours are removed, more -> solid
    void
    add_simple_rules
    (
        Rules* ca,
        int times,
        int count,
        bool include_self,
        NeighborhoodAlgorithm algorithm
    );

    /// blanks out cells at 'y' and 'height' down
    void
    add_horizontal_blank_rule
    (
        Rules* ca,
        int y,
        int height
    );

    /// simple smoothing, but always set the cell, seems to provide spikes
    void
    add_spiky_rules
    (
        Rules* ca,
        int times,
        int count,
        bool include_self,
        NeighborhoodAlgorithm algorithm
    );

    /// smooth but 'big_count' is applied for R(2)
    void
    add_combo_rules
    (
        Rules* ca,
        int times,
        int count,
        int big_count,
        bool include_self,
        NeighborhoodAlgorithm algorithm
    );

    /// fills all open areas that are less than 'min_count'
    void
    add_fill_small_holes_rule(Rules* rules, bool allow_diagonals, int min_count);

    /// fills all open areas, but keeps 'holes_to_keep'
    void
    add_fill_all_holes_rule(Rules* rules, bool allow_diagonals, int holes_to_keep);
}
