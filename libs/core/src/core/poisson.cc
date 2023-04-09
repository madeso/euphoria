#include "core/poisson.h"

#include "core/numeric.h"
#include "core/random.h"
#include "assert/assert.h"
#include "core/cint.h"


namespace euphoria::core
{
    // todo(Gustav): verify samples is inside Rect and considers the offset of the Rect

    bool
    is_all_inside(const Rectf& a, const vec2f& p, float r)
    {
        return
            a.left < p.x - r &&
            a.right > p.x + r &&
            a.top > p.y + r &&
            a.bottom < p.y - r;
    }

    PoissonWorker::PoissonWorker(const Rectf& aarea, Random* arandom, float ar, float bs, int ak)
        : area(aarea)
        , rand(arandom)
        , r(ar)
        , bounds_check(bs)
        , k(ak)
        , w(r / sqrt(2))
        , grid(Table<int>::from_width_height(floor_to_int(area.get_width()/w), floor_to_int(area.get_height()/w), -1))
    {
        auto p = get_random_point_in_area();
        if(bounds_check > 0)
        {
            for(int try_index=0; try_index<k; try_index+=1)
            {
                if(is_all_inside(area, p, bounds_check))
                {
                    break;
                }
                else
                {
                    p = get_random_point_in_area();
                }
            }
        }
        const auto i = c_point_to_index(p);
        active.emplace_back(0);
        samples.emplace_back(p);
        grid[{i.x, i.y}] = 0;
    }


    vec2f
    PoissonWorker::get_random_point_in_area() const
    { return get_random_point(rand, area); }


    vec2i
    PoissonWorker::c_point_to_index(const vec2f& p) const
    { return vec2i{ floor_to_int(p.x/w), floor_to_int(p.y/w) }; }


    bool
    PoissonWorker::can_place_at(const vec2f& potential_sample, const vec2i& potential_sample_pos)
    {
        const int range = 3;
        for(int dy=-range; dy<=range; dy+=1)
        {
            for(int dx=-range; dx<=range; dx+=1)
            {
                const auto neighbour_pos = potential_sample_pos + vec2i{dx, dy};
                if(!grid.is_inside(neighbour_pos.x, neighbour_pos.y)) { continue; }
                const auto neighbour_sample_index = grid[{neighbour_pos.x, neighbour_pos.y}];
                if(neighbour_sample_index == -1) { continue; }
                const auto d2 = vec2f::from_to(samples[neighbour_sample_index], potential_sample).get_length_squared();
                if(d2 <= square(r))
                {
                    return false;
                }
            }
        }

        return true;
    }


    std::tuple<bool, vec2f>
    PoissonWorker::try_place(int active_index)
    {
        const auto base_sample = samples[active[active_index]];

        for(int try_index = 0; try_index<k; try_index +=1)
        {
            const auto unit = create_random_unit(rand);
            const auto random_range = get_random_in_range(rand, r, 2*r);
            const auto sample = base_sample + unit * random_range;
            const auto sample_pos = c_point_to_index(sample);

            if(!grid.is_inside(sample_pos.x, sample_pos.y)) { try_index -=1; continue;}

            if(bounds_check > 0)
            {
                if(!is_all_inside(area, sample, bounds_check)){ continue;}
            }

            if(can_place_at(sample, sample_pos))
            {
                const auto point_index = c_sizet_to_int(samples.size());

                ASSERT((grid[{sample_pos.x, sample_pos.y}]) == -1);

                samples.emplace_back(sample);
                grid[{sample_pos.x, sample_pos.y}] = point_index;
                active.emplace_back(point_index);

                return {true, sample};
            }
        }

        return {false, zero2f};
    }


    bool
    PoissonWorker::is_done() const
    {
        return active.empty();
    }


    std::optional<std::tuple<vec2f, vec2f>>
    PoissonWorker::update()
    {
        if(active.empty())
        {
            return std::nullopt;
        }

        const auto active_index = get_random_in_range(rand, active.size());

        const auto [placed, sample] = try_place(c_sizet_to_int(active_index));
        if(placed)
        {
            const vec2f from = samples[active[active_index]];
            const vec2f to = sample; //*samples.rbegin();
            return std::make_tuple(from, to);
        }
        else
        {
            active.erase(std::next(active.begin(), c_sizet_to_int(active_index)));
            return std::nullopt;
        }
    }


    std::vector<vec2f>
    calc_poisson_samples(const Rectf& area, Random* random, float r, float bs, int k)
    {
        auto worker = PoissonWorker{area, random, r, bs, k};
        while(!worker.is_done())
        {
            worker.update();
        }
        return worker.samples;
    }
}
