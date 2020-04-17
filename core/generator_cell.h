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


    struct SmoothRule : public Rule
    {
        using SmoothFunction = std::function
        <
            std::optional<bool>
            (
                const Wallcounter&
            )
        >;

        SmoothFunction smooth_function;

        explicit SmoothRule(SmoothFunction sf);

        void Step(CellularAutomata* self) override;
    };


    struct RandomFillRule : public Rule
    {
        Random* random;
        float random_fill;
        Fourway<BorderSetupRule> border_control;

        RandomFillRule(Random* r, float rf, Fourway<BorderSetupRule> bc);
        void Step(CellularAutomata* self) override;
    };

    // struct VerticalBlankRule : public Rule
    // {
    //     void Step(CellularAutomata* self) override;
    // };

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
    AddClearRules(Rules* ca, int times, int count, int range);

    void
    AddSimpleRules(Rules* ca, int times, int count);

    void
    AddSpikyRules(Rules* ca, int times, int count);

    void
    AddComboRules(Rules* ca, int times, int count, int big_count);
}

#endif  // CORE_GENERATOR_CELL_H
