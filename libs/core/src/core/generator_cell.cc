#include "core/generator_cell.h"

#include "base/random.h"
#include "core/image_draw.h"
#include "core/colors.h"
#include "core/image.h"
#include "base/cint.h"
#include "core/table_bool.h"

#include <functional>
#include <optional>

namespace eu::core::generator
{
    struct SmoothRule final : public Rule
    {
        using SmoothFunction = ChangeFunction;

        SmoothFunction smooth_function;

        explicit SmoothRule(SmoothRule::SmoothFunction sf)
            : smooth_function(sf)
        {
        }

        void
        update(CellularAutomata* self) override
        {
            make_smoother
            (
                self->world,
                self->outside_rule,
                smooth_function
            );
        }
    };


    struct RandomFillRule : public Rule
    {
        Random* rand;
        float random_fill;
        Lrud<BorderSetupRule> border_control;

        RandomFillRule
        (
            Random* r,
            float rf,
            Lrud<BorderSetupRule> bc
        )
            : rand(r)
            , random_fill(rf)
            , border_control(bc)
        {
        }

        void
        update(CellularAutomata* self) override
        {
            set_white_noise(self->world, border_control, [this]()
            {
                return rand->get_next_float01() < random_fill;
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
        update(CellularAutomata* self) override
        {
            BoolTable& world = *self->world;
            const auto regions = find_empty_regions(world, allow_diagonals);
            for(const auto& re: regions)
            {
                if(c_sizet_to_int(re.size()) < min_count)
                {
                    for(const auto& p: re)
                    {
                        world[{p.x, p.y}] = true;
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
        update(CellularAutomata* self) override
        {
            BoolTable& world = *self->world;
            auto regions = find_empty_regions(world, allow_diagonals);
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
            for(int hole_index=0; hole_index<holes_to_keep && regions.empty() == false; hole_index+=1)
            {
                regions.pop_back();
            }
            for(const auto& re: regions)
            {
                for(const auto& p: re)
                {
                    world[{p.x, p.y}] = true;
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
        update(CellularAutomata* self) override
        {
            const BoolTable current = *self->world;

            self->world->set_all
            (
                [&current, this] (int x, int y)
                {
                    if(y >= center && y < center + height)
                    {
                        // todo(Gustav): this doesn't respect the outside rule
                        // but should it...?
                        return false;
                    }
                    return current[{x, y}];
                }
            );
        }
    };


    void
    Rules::add_rule(int count, std::shared_ptr<Rule> rule)
    {
        for (int index = 0; index < count; index += 1)
        {
            rules.emplace_back(rule);
        }
    }


    CellularAutomata::CellularAutomata
    (
        generator::Rules* r,
        generator::World* w,
        const Lrud<core::OutsideRule>& fw
    )
        : rules(r)
        , world(w)
        , outside_rule(fw)
        , iteration(0)
    {
    }


    bool
    CellularAutomata::is_done() const
    {
        return false == (iteration < c_sizet_to_int(rules->rules.size()));
    }


    void
    CellularAutomata::update()
    {
        rules->rules[iteration]->update(this);
        iteration += 1;
    }


    void
    add_random_fill
    (
        Rules* cell,
        Random* random,
        float random_fill,
        Lrud<BorderSetupRule> border_control
    )
    {
        cell->add_rule
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
    add_clear_rules(Rules* ca, int times, int count, int range, bool include_self, NeighborhoodAlgorithm algorithm)
    {
        ca->add_rule
        (
            times,
            std::make_shared<SmoothRule>
            (
                [count, range, include_self, algorithm] (bool, const WallCounter& wc) -> std::optional<bool>
                {
                    const auto walls = wc.count(range, include_self, algorithm);
                    if (walls < count) { return false; }
                    return std::nullopt;
                }
            )
        );
    }


    void
    add_complex_rules
    (
        Rules* ca,
        int times,
        ChangeFunction change
    )
    {
        ca->add_rule
        (
            times,
            std::make_shared<SmoothRule>(change)
        );
    }


    void
    add_simple_rules(Rules* ca, int times, int count, bool include_self, NeighborhoodAlgorithm algorithm)
    {
        ca->add_rule
        (
            times,
            std::make_shared<SmoothRule>
            (
                [count, include_self, algorithm] (bool, const WallCounter& wc) -> std::optional<bool>
                {
                    const auto walls = wc.count(1, include_self, algorithm);
                    if (walls > count) { return true; }
                    if (walls < count) { return false; }
                    return std::nullopt;
                }
            )
        );
    }


    void
    add_horizontal_blank_rule(Rules* ca, int y, int height)
    {
        ca->add_rule(1, std::make_shared<HorizontalBlankRule>(y, height));
    }


    void
    add_spiky_rules(Rules* ca, int times, int count, bool include_self, NeighborhoodAlgorithm algorithm)
    {
        ca->add_rule
        (
            times,
            std::make_shared<SmoothRule>
            (
                [count, include_self, algorithm] (bool, const WallCounter& wc) -> std::optional<bool>
                {
                    return wc.count(1, include_self, algorithm) >= count;
                }
            )
        );
    }


    void
    add_combo_rules(Rules* ca, int times, int count, int big_count, bool include_self, NeighborhoodAlgorithm algorithm)
    {
        ca->add_rule
        (
            times,
            std::make_shared<SmoothRule>
            (
                [count, big_count, include_self, algorithm](bool, const WallCounter& wc) -> std::optional<bool>
                {
                    const auto walls = wc.count(1, include_self, algorithm);
                    if (walls > count) { return true; }
                    if(wc.count(2, include_self, algorithm) <= big_count) { return true; }
                    if (walls < count) { return false; }
                    return std::nullopt;
                }
            )
        );
    }


    void
    add_fill_small_holes_rule(Rules* rules, bool allow_diagonals, int min_count)
    {
        rules->add_rule
        (
            1,
            std::make_shared<FillSmallHolesRule>(allow_diagonals, min_count)
        );
    }


    void
    add_fill_all_holes_rule(Rules* rules, bool allow_diagonals, int holes_to_keep)
    {
        rules->add_rule
        (
            1,
            std::make_shared<FillAllHoles>(allow_diagonals, holes_to_keep)
        );
    }
}
