#include "core/poisson.h"

#include "core/numeric.h"
#include "core/random.h"
#include "core/assert.h"

namespace euphoria::core
{
    // todo(Gustav): verify samples is inside rect and considers the offset of the rect

    PoissonWorker::PoissonWorker(const Rectf& aarea, Random* arandom, float ar, int ak)
        : area(aarea)
        , random(arandom)
        , r(ar)
        , k(ak)
        , w(r / Sqrt(2))
        , grid(Table<int>::FromWidthHeight(Floori(area.GetWidth()/w), Floori(area.GetHeight()/w), -1))
    {
        const auto p = random_point();
        const auto i = point_to_index(p);
        active.emplace_back(0);
        samples.emplace_back(p);
        grid.Value(i.x, i.y, 0);
    }


    vec2f
    PoissonWorker::random_point()
    { return area.RandomPoint(random); }


    vec2i
    PoissonWorker::point_to_index(const vec2f& p)
    { return vec2i{ Floori(p.x/w), Floori(p.y/w) }; };


    bool
    PoissonWorker::can_place_at(const vec2f& potential_sample, const vec2i& potential_sample_pos)
    {
        const int range = 1;
        for(int dy=-range; dy<=range; dy+=1)
        {
            for(int dx=-range; dx<=range; dx+=1)
            {
                const auto neighbour_pos = potential_sample_pos + vec2i{dx, dy};
                if(!grid.IsInside(neighbour_pos.x, neighbour_pos.y)) continue;
                const auto neighbour_sample_index = grid.Value(neighbour_pos.x, neighbour_pos.y);
                if(neighbour_sample_index == -1) continue;
                const auto d2 = vec2f::FromTo(samples[neighbour_sample_index], potential_sample).GetLengthSquared();
                if(d2 <= Square(r))
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
            const auto unit = RandomUnit(random);
            const auto random_range = random->NextRange(r, 2*r);
            const auto sample = base_sample + unit * random_range;
            const auto sample_pos = point_to_index(sample);

            if(!grid.IsInside(sample_pos.x, sample_pos.y)) { try_index -=1; continue;}

            if(can_place_at(sample, sample_pos))
            {
                const auto point_index = samples.size();

                ASSERT( grid.Value(sample_pos.x, sample_pos.y) == -1);

                samples.emplace_back(sample);
                grid.Value(sample_pos.x, sample_pos.y, point_index);
                active.emplace_back(point_index);

                return {true, sample};
            }
        }

        return {false, vec2f::Zero()};
    }


    bool
    PoissonWorker::IsDone() const
    {
        return active.empty();
    }


    std::optional<std::tuple<vec2f, vec2f>>
    PoissonWorker::Step()
    {
        if(active.empty())
        {
            return std::nullopt;
        }

        const auto active_index = random->NextRange(active.size());
        
        const auto [placed, sample] = try_place(active_index);
        if(placed)
        {
            const vec2f from = samples[active[active_index]];
            const vec2f to = sample; //*samples.rbegin();
            return std::make_tuple(from, to);
        }
        else
        {
            active.erase(active.begin() + active_index);
            return std::nullopt;
        }
    }


    std::vector<vec2f>
    PoissonSample(const Rectf& area, Random* random, float r, int k)
    {
        auto worker = PoissonWorker{area, random, r, k};
        while(!worker.IsDone())
        {
            worker.Step();
        }
        return worker.samples;
    }
}
