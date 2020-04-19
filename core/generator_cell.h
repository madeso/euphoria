#ifndef CORE_GENERATOR_CELL_H
#define CORE_GENERATOR_CELL_H

#include "core/fourway.h"
#include "core/table_bool.h"

#include <vector>

namespace euphoria::core
{
    struct Random;
}

namespace euphoria::core::generator
{
    using World = BoolTable;

    // make generation better
    // http://www.roguebasin.com/index.php?title=Cellular_Automata_Method_for_Generating_Random_Cave-Like_Levels

    struct CellularAutomata;

    struct Rule
    {
        virtual ~Rule();
        virtual void Step(CellularAutomata* self) = 0;
    };


    struct Rules
    {
        std::vector<std::shared_ptr<Rule>> rules;

        void
        AddRule(int count, std::shared_ptr<Rule> rule);
    };

    struct CellularAutomata
    {
        Rules* rules;
        World* world;
        Fourway<OutsideRule> outside_rule;
        int iteration;

        CellularAutomata(Rules* r, World* w, const Fourway<OutsideRule>& fw);

        bool
        HasMoreWork() const;

        void
        Work();
    };

    // todo(Gustav): rename rules to make it clearer what they do

    /// fills the world with random data
    void
    AddRandomFill
    (
        Rules* cell,
        Random* random,
        float random_fill = 0.5,
        Fourway<BorderSetupRule> border_control = Fourway<BorderSetupRule>
        {
            BorderSetupRule::AlwaysWall
        }
    );

    /// 'clears' cells with less than 'count' neighbours, good for removing 'blobs'
    void
    AddClearRules
    (
        Rules* ca,
        int times,
        int count,
        int range,
        bool include_self,
        NeighborhoodAlgorithm algorithm
    );

    /// simple smoothing, less than count neighbours are removed, more -> solid
    void
    AddSimpleRules
    (
        Rules* ca,
        int times,
        int count,
        bool include_self,
        NeighborhoodAlgorithm algorithm
    );

    /// blanks out cells at 'y' and 'height' down
    void
    AddHorizontalBlankRule
    (
        Rules* ca,
        int y,
        int height
    );

    /// simple smoothing, but always set the cell, seems to provide spikes
    void
    AddSpikyRules
    (
        Rules* ca,
        int times,
        int count,
        bool include_self,
        NeighborhoodAlgorithm algorithm
    );

    /// smooth but 'big_count' is applied for R(2)
    void
    AddComboRules
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
    AddFillSmallHolesRule(Rules* rules, bool allow_diagonals, int min_count);

    /// fills all open areas, but keeps 'holes_to_keep'
    void
    AddFillAllHolesRule(Rules* rules, bool allow_diagonals, int holes_to_keep);
}

#endif  // CORE_GENERATOR_CELL_H
