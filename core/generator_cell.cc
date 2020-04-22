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
        using SmoothFunction = ChangeFunction;

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


    struct FillSmallHolesRule : public Rule
    {
        bool allow_diagonals;
        int min_count;

        FillSmallHolesRule(bool ad, int mc)
            : allow_diagonals(ad)
            , min_count(mc)
        {
        }

        void
        Step(CellularAutomata* self) override
        {
            BoolTable& world = *self->world;
            const auto regions = FindEmptyRegions(world, allow_diagonals);
            for(const auto& re: regions)
            {
                if( re.size() < min_count)
                {
                    for(const auto& p: re)
                    {
                        world(p.x, p.y) = true;
                    }
                }
            }
        }
    };


    struct FillAllHoles : public Rule
    {
        bool allow_diagonals;
        int holes_to_keep;

        FillAllHoles(bool ad, int htk)
            : allow_diagonals(ad)
            , holes_to_keep(htk)
        {
        }

        void
        Step(CellularAutomata* self) override
        {
            BoolTable& world = *self->world;
            auto regions = FindEmptyRegions(world, allow_diagonals);
            using V = std::vector<vec2i>;
            std::sort
            (
                regions.begin(),
                regions.end(),
                [](const V& lhs, const V& rhs)
                {
                    return lhs.size() < rhs.size();
                }
            );
            for(int h=0; h<holes_to_keep && regions.empty() == false; h+=1)
            {
                regions.pop_back();
            }
            for(const auto& re: regions)
            {
                for(const auto& p: re)
                {
                    world(p.x, p.y) = true;
                }
            }
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
    AddClearRules(Rules* ca, int times, int count, int range, bool include_self, NeighborhoodAlgorithm algorithm)
    {
        ca->AddRule
        (
            times,
            std::make_shared<SmoothRule>
            (
                [count, range, include_self, algorithm] (bool, const Wallcounter& wc) -> std::optional<bool>
                {
                    const auto walls = wc.Count(range, include_self, algorithm);
                    if (walls < count) { return false; }
                    return std::nullopt;
                }
            )
        );
    }


    void
    AddComplexRules
    (
        Rules* ca,
        int times,
        ChangeFunction change
    )
    {
        ca->AddRule
        (
            times,
            std::make_shared<SmoothRule>(change)
        );
    }


    void
    AddSimpleRules(Rules* ca, int times, int count, bool include_self, NeighborhoodAlgorithm algorithm)
    {
        ca->AddRule
        (
            times,
            std::make_shared<SmoothRule>
            (
                [count, include_self, algorithm] (bool, const Wallcounter& wc) -> std::optional<bool>
                {
                    const auto walls = wc.Count(1, include_self, algorithm);
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
    AddSpikyRules(Rules* ca, int times, int count, bool include_self, NeighborhoodAlgorithm algorithm)
    {
        ca->AddRule
        (
            times,
            std::make_shared<SmoothRule>
            (
                [count, include_self, algorithm] (bool, const Wallcounter& wc) -> std::optional<bool>
                {
                    return wc.Count(1, include_self, algorithm) >= count;
                }
            )
        );
    }


    void
    AddComboRules(Rules* ca, int times, int count, int big_count, bool include_self, NeighborhoodAlgorithm algorithm)
    {
        ca->AddRule
        (
            times,
            std::make_shared<SmoothRule>
            (
                [count, big_count, include_self, algorithm](bool, const Wallcounter& wc) -> std::optional<bool>
                {
                    const auto walls = wc.Count(1, include_self, algorithm);
                    if (walls > count) { return true; }
                    if(wc.Count(2, include_self, algorithm) <= big_count) { return true; }
                    if (walls < count) { return false; }
                    return std::nullopt;
                }
            )
        );
    }


    void
    AddFillSmallHolesRule(Rules* rules, bool allow_diagonals, int min_count)
    {
        rules->AddRule
        (
            1,
            std::make_shared<FillSmallHolesRule>(allow_diagonals, min_count)
        );
    }


    void
    AddFillAllHolesRule(Rules* rules, bool allow_diagonals, int holes_to_keep)
    {
        rules->AddRule
        (
            1,
            std::make_shared<FillAllHoles>(allow_diagonals, holes_to_keep)
        );
    }
}  // namespace euphoria::core
