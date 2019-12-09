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
    PoissonWorker::can_place_at(const vec2f& sample, const vec2i& sample_index)
    {
        for(int dy=-1; dy<=1; dy+=1)
        {
            for(int dx=-1; dx<=1; dx+=1)
            {
                const auto check_index = sample_index + vec2i{dx, dy};
                if(!grid.IsInside(check_index.x, check_index.y)) continue;
                const auto potential_active_index = grid.Value(check_index.x, check_index.y);
                if(potential_active_index == -1) continue;
                const auto d2 = vec2f::FromTo(samples[potential_active_index], sample).GetLengthSquared();
                if(d2 < Square(r))
                {
                    return false;
                }
            }
        }

        return true;
    }


    bool
    PoissonWorker::try_place(int active_index)
    {
        const auto base_sample = samples[active[active_index]];

        for(int try_index = 0; try_index<k; try_index +=1)
        {
            const auto unit = RandomUnit(random);
            const auto random_range = random->NextRange(r, 2*r);
            const auto sample = base_sample + unit * random_range;
            const auto sample_index = point_to_index(sample);

            if(!grid.IsInside(sample_index.x, sample_index.y)) { try_index -=1; continue;}

            if(can_place_at(sample, sample_index))
            {
                const auto point_index = samples.size();

                samples.emplace_back(sample);
                ASSERT( grid.Value(sample_index.x, sample_index.y) == -1);
                grid.Value(sample_index.x, sample_index.y, point_index);
                active.emplace_back(point_index);

                return true;
            }
        }

        return false;
    }


    bool
    PoissonWorker::IsDone() const
    {
        return active.empty();
    }
    

    void
    PoissonWorker::Step()
    {
        if(active.empty())
        {
            return;
        }

        const auto active_index = random->NextRange(active.size());
        
        const auto placed = try_place(active_index);
        if(!placed)
        {
            active.erase(active.begin() + active_index);
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
