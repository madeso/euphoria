#include "core/generator_cell.h"

#include "core/random.h"
#include "core/image_draw.h"
#include "core/colors.h"
#include "core/image.h"
#include "core/cint.h"
#include "core/table_bool.h"

#include <functional>
#include <optional>

namespace euphoria::core::generator
{
    struct smooth_rule final : public rule
    {
        using SmoothFunction = change_function;

        SmoothFunction smooth_function;

        explicit smooth_rule(smooth_rule::SmoothFunction sf)
            : smooth_function(sf)
        {
        }

        void
        step(cellular_automata* self) override
        {
            smooth_map
            (
                self->world,
                self->outside_rule,
                smooth_function
            );
        }
    };


    struct random_fill_rule : public rule
    {
        Random* random;
        float random_fill;
        fourway<border_setup_rule> border_control;

        random_fill_rule
        (
            Random* r,
            float rf,
            fourway<border_setup_rule> bc
        )
            : random(r)
            , random_fill(rf)
            , border_control(bc)
        {
        }

        void
        step(cellular_automata* self) override
        {
            set_white_noise(self->world, border_control, [this]()
            {
                return random->NextFloat01() < random_fill;
            });
        }
    };


    struct fill_small_holes_rule : public rule
    {
        bool allow_diagonals;
        int min_count;

        fill_small_holes_rule(bool ad, int mc)
            : allow_diagonals(ad)
            , min_count(mc)
        {
        }

        void
        step(cellular_automata* self) override
        {
            bool_table& world = *self->world;
            const auto regions = find_empty_regions(world, allow_diagonals);
            for(const auto& re: regions)
            {
                if( Csizet_to_int(re.size()) < min_count)
                {
                    for(const auto& p: re)
                    {
                        world(p.x, p.y) = true;
                    }
                }
            }
        }
    };


    struct fill_all_holes : public rule
    {
        bool allow_diagonals;
        int holes_to_keep;

        fill_all_holes(bool ad, int htk)
            : allow_diagonals(ad)
            , holes_to_keep(htk)
        {
        }

        void
        step(cellular_automata* self) override
        {
            bool_table& world = *self->world;
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


    struct horizontal_blank_rule : public rule
    {
        int center;
        int height;

        horizontal_blank_rule(int c, int h)
            : center(c)
            , height(h)
        {
        }

        void
        step(cellular_automata* self) override
        {
            const bool_table current = *self->world;

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
                    return current(x, y);
                }
            );
        }
    };


    void
    rules::add_rule(int count, std::shared_ptr<rule> rule)
    {
        for (int i = 0; i < count; i += 1)
        {
            rules.emplace_back(rule);
        }
    }


    cellular_automata::cellular_automata
    (
        generator::rules* r,
        generator::world* w,
        const fourway<core::outside_rule>& fw
    )
        : rules(r)
        , world(w)
        , outside_rule(fw)
        , iteration(0)
    {
    }


    bool
    cellular_automata::has_more_work() const
    {
        return iteration < Csizet_to_int(rules->rules.size());
    }


    void
    cellular_automata::work()
    {
        rules->rules[iteration]->step(this);
        iteration += 1;
    }


    void
    add_random_fill
    (
        rules* cell,
        Random* random,
        float random_fill,
        fourway<border_setup_rule> border_control
    )
    {
        cell->add_rule
        (
            1,
            std::make_shared<random_fill_rule>
            (
                random,
                random_fill,
                border_control
            )
        );
    }


    void
    add_clear_rules(rules* ca, int times, int count, int range, bool include_self, neighborhood_algorithm algorithm)
    {
        ca->add_rule
        (
            times,
            std::make_shared<smooth_rule>
            (
                [count, range, include_self, algorithm] (bool, const wallcounter& wc) -> std::optional<bool>
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
        rules* ca,
        int times,
        change_function change
    )
    {
        ca->add_rule
        (
            times,
            std::make_shared<smooth_rule>(change)
        );
    }


    void
    add_simple_rules(rules* ca, int times, int count, bool include_self, neighborhood_algorithm algorithm)
    {
        ca->add_rule
        (
            times,
            std::make_shared<smooth_rule>
            (
                [count, include_self, algorithm] (bool, const wallcounter& wc) -> std::optional<bool>
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
    add_horizontal_blank_rule(rules* ca, int y, int height)
    {
        ca->add_rule(1, std::make_shared<horizontal_blank_rule>(y, height));
    }


    void
    add_spiky_rules(rules* ca, int times, int count, bool include_self, neighborhood_algorithm algorithm)
    {
        ca->add_rule
        (
            times,
            std::make_shared<smooth_rule>
            (
                [count, include_self, algorithm] (bool, const wallcounter& wc) -> std::optional<bool>
                {
                    return wc.count(1, include_self, algorithm) >= count;
                }
            )
        );
    }


    void
    add_combo_rules(rules* ca, int times, int count, int big_count, bool include_self, neighborhood_algorithm algorithm)
    {
        ca->add_rule
        (
            times,
            std::make_shared<smooth_rule>
            (
                [count, big_count, include_self, algorithm](bool, const wallcounter& wc) -> std::optional<bool>
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
    add_fill_small_holes_rule(rules* rules, bool allow_diagonals, int min_count)
    {
        rules->add_rule
        (
            1,
            std::make_shared<fill_small_holes_rule>(allow_diagonals, min_count)
        );
    }


    void
    add_fill_all_holes_rule(rules* rules, bool allow_diagonals, int holes_to_keep)
    {
        rules->add_rule
        (
            1,
            std::make_shared<fill_all_holes>(allow_diagonals, holes_to_keep)
        );
    }
}
