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

    // todo(Gustav): Skip basic rules setup and improve argparse
    // so worldgen sample can be improved

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

    void
    AddSimpleRules
    (
        Rules* ca,
        int times,
        int count,
        bool include_self,
        NeighborhoodAlgorithm algorithm
    );

    void
    AddHorizontalBlankRule
    (
        Rules* ca,
        int y,
        int height
    );

    void
    AddSpikyRules
    (
        Rules* ca,
        int times,
        int count,
        bool include_self,
        NeighborhoodAlgorithm algorithm
    );

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
}

#endif  // CORE_GENERATOR_CELL_H
