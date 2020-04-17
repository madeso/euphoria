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

        explicit SmoothRule(SmoothRule::SmoothFunction sf)
            : smooth_function(sf)
        {
        }

        void
        Step(CellularAutomata* self) override
        {
            SmoothMap
            (
                self->world,
                self->outside_rule,
                smooth_function
            );
        }
    };


    struct RandomFillRule : public Rule
    {
        Random* random;
        float random_fill;
        Fourway<BorderSetupRule> border_control;

        RandomFillRule
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
        Step(CellularAutomata* self) override
        {
            SetWhiteNoise(self->world, border_control, [this]()
            {
                return random->NextFloat01() < random_fill;
            });
        }
    };

    struct HorizontalBlankRule : public Rule
    {
        int center;
        int height;

        HorizontalBlankRule(int c, int h)
            : center(c)
            , height(h)
        {
        }

        void
        Step(CellularAutomata* self) override
        {
            const BoolTable current = *self->world;

            self->world->SetAll
            (
                [&current, this] (int x, int y)
                {
                    if(y >= center && y < center + height)
                    {
                        // todo(Gustav): this doesn't respect the outside rule
                        // but should it...?
                        return false;
                    }
                    return current(x, y);
                }
            );
        }
    };


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
    AddClearRules(Rules* ca, int times, int count, int range)
    {
        ca->AddRule
        (
            times,
            std::make_shared<SmoothRule>
            (
                [count, range] (const Wallcounter& wc) -> std::optional<bool>
                {
                    const auto walls = wc.Count(range);
                    if (walls < count) { return false; }
                    return std::nullopt;
                }
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
    AddHorizontalBlankRule(Rules* ca, int y, int height)
    {
        ca->AddRule(1, std::make_shared<HorizontalBlankRule>(y, height));
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
        ca->AddRule
        (
            times,
            std::make_shared<SmoothRule>
            (
                [count, big_count](const Wallcounter& wc) -> std::optional<bool>
                {
                    const auto walls = wc.Count(1);
                    if (walls > count) { return true; }
                    if(wc.Count(2) <= big_count) { return true; }
                    if (walls < count) { return false; }
                    return std::nullopt;
                }
            )
        );
    }
}  // namespace euphoria::core
