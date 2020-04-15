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


    SmoothRule::SmoothRule(SmoothRule::SmoothFunction sf)
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


    RandomFillRule::RandomFillRule
    (
        Random* r,
        float rf,
        Fourway<BorderSetupRule> bc
    )
        : random(r)
        , random_fill(rf)
        , border_control(bc)
    {
    }

    void
    RandomFillRule::Step(CellularAutomata* self)
    {
        SetWhiteNoise(self->world, border_control, [this]()
        {
            return random->NextFloat01() < random_fill;
        });
    }


    void
    Rules::AddRule(int count, std::shared_ptr<Rule> rule)
    {
        for (int i = 0; i < count; i += 1)
        {
            rules.emplace_back(rule);
        }
    }


    CellularAutomata::CellularAutomata
    (
        Rules* r,
        World* w,
        const Fourway<OutsideRule>& fw
    )
        : rules(r)
        , world(w)
        , outside_rule(fw)
        , iteration(0)
    {
    }


    bool
    CellularAutomata::HasMoreWork() const
    {
        return iteration < rules->rules.size();
    }


    void
    CellularAutomata::Work()
    {
        rules->rules[iteration]->Step(this);
        iteration += 1;
    }


    void
    AddRandomFill
    (
        Rules* cell,
        Random* random,
        float random_fill,
        Fourway<BorderSetupRule> border_control
    )
    {
        cell->AddRule
        (
            1,
            std::make_shared<RandomFillRule>
            (
                random,
                random_fill,
                border_control
            )
        );
    }


    void
    AddSimpleRules(Rules* ca, int times, int count)
    {
        ca->AddRule
        (
            times,
            std::make_shared<SmoothRule>
            (
                [count] (const Wallcounter& wc) -> std::optional<bool>
                {
                    const auto walls = wc.Count(1);
                    if (walls > count) { return true; }
                    if (walls < count) { return false; } 
                    return std::nullopt;
                }
            )
        );
    }


    void
    AddSpikyRules(Rules* ca, int times, int count)
    {
        ca->AddRule
        (
            times,
            std::make_shared<SmoothRule>
            (
                [count] (const Wallcounter& wc) -> std::optional<bool>
                {
                    return wc.Count(1) >= count;
                }
            )
        );
    }


    void
    AddComboRules(Rules* ca, int times, int count, int big_count)
    {
        // also produces spikes
        ca->AddRule
        (
            times,
            std::make_shared<SmoothRule>
            (
                [count, big_count](const Wallcounter& wc) -> std::optional<bool>
                {
                    return wc.Count(1) >= count || wc.Count(2) <= big_count;
                }
            )
        );
    }
}  // namespace euphoria::core
