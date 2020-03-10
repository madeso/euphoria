#include "core/generator_cell.h"

#include "core/random.h"
#include "core/image_draw.h"
#include "core/colors.h"
#include "core/image.h"
#include "core/table_bool.h"

#include <functional>
#include <optional>

namespace euphoria::core::generator
{
    Rule::~Rule()
    {
    }


    SmoothRule::SmoothRule(std::function<std::optional<bool>(const Wallcounter&)> sf)
        : smooth_function(sf)
    {
    }


    void
    SmoothRule::Step(CellularAutomata* self)
    {
        SmoothMap
        (
            self->world,
            self->outside_rule,
            smooth_function
        );
    }


    CellularAutomata&
    CellularAutomata::AddRule(int count, std::shared_ptr<Rule> rule)
    {
        for (int i = 0; i < count; i += 1)
        {
            rules.emplace_back(rule);
        }

        return *this;
    }


    void
    CellularAutomata::Setup()
    {
        SetWhiteNoise(world, border_control, [&]()
        {
            return random->NextFloat01() < random_fill;
        });
    }


    bool
    CellularAutomata::HasMoreWork() const
    {
        return iteration < rules.size();
    }


    void
    CellularAutomata::Work()
    {
        rules[iteration]->Step(this);
        iteration += 1;
    }

    // todo(Gustav): make theese rule buildings part of the cellular automata setup
    // along with setup step

    void
    SetupSimpleRules(CellularAutomata* ca)
    {
        ca->rules.clear();
        ca->AddRule(5, std::make_shared<SmoothRule>([](const Wallcounter& wc) -> std::optional<bool>
        {
            const auto walls = wc.Count(1);
            if (walls > 4) return true;
            if (walls < 4) return false;
            return std::nullopt;
        }));
    }

    void
    SetupBasicRules(CellularAutomata* ca)
    {
        ca->rules.clear();
        ca->AddRule(5, std::make_shared<SmoothRule>([](const Wallcounter& wc) -> std::optional<bool>
        {
            return wc.Count(1) >= 5;
        }));
    }

    void
    SetupNoEmptyAreas(CellularAutomata* ca)
    {
        ca->rules.clear();
        ca->AddRule(4, std::make_shared<SmoothRule>([](const Wallcounter& wc) -> std::optional<bool>
        {
            return wc.Count(1) >= 5 || wc.Count(2) <= 2;
        }));
        ca->AddRule(3, std::make_shared<SmoothRule>([](const Wallcounter& wc) -> std::optional<bool>
        {
            return wc.Count(1) >= 5;
        }));
    }
}  // namespace euphoria::core
