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
        std::function<std::optional<bool>(const Wallcounter&)> smooth_function;

        explicit SmoothRule(std::function<std::optional<bool>(const Wallcounter&)> sf);

        void Step(CellularAutomata* self) override;
    };


    // struct VerticalBlankRule : public Rule
    // {
    //     void Step(CellularAutomata* self) override;
    // };

    struct CellularAutomata
    {
        World*  world  = nullptr;

        Random* random = nullptr;
        float random_fill = 0.5;
        Fourway<BorderSetupRule> border_control = Fourway<BorderSetupRule>{ BorderSetupRule::AlwaysWall };

        Fourway<OutsideRule> outside_rule = Fourway<OutsideRule>{ OutsideRule::Empty };
        std::vector<std::shared_ptr<Rule>> rules;

        CellularAutomata&
        AddRule(int count, std::shared_ptr<Rule> rule);

        int iteration = 0;

        void
        Setup();

        bool
        HasMoreWork() const;

        void
        Work();
    };

    // todo(Gustav): Skip basic rules setup and improve argparse
    // so worldgen sample can be improved

    void
    SetupSimpleRules(CellularAutomata* ca);

    void
    SetupBasicRules(CellularAutomata* ca);

    void
    SetupNoEmptyAreas(CellularAutomata* ca);
}

#endif  // CORE_GENERATOR_CELL_H
