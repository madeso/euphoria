#include "core/poisson.h"

#include "core/numeric.h"
#include "core/table.h"
#include "core/random.h"
#include "core/assert.h"

namespace euphoria::core
{
    std::vector<vec2f>
    PoissonSample(const Rectf& area, Random* random, float r, int k)
    {
        const auto w = r / Sqrt(2);

        auto random_point = [&]() { return area.RandomPoint(random); };
        auto point_to_index = [&](const vec2f& p)
            { return vec2i{ Floori(p.x/w), Floori(p.y/w) }; };

        const auto cols = Floori(area.GetWidth() / w);
        const auto rows = Floori(area.GetHeight() / w);
        
        auto grid    = Table<int>::FromWidthHeight(cols, rows, -1);
        auto active  = std::vector<int>{};
        auto samples = std::vector<vec2f>{};

        {
            const auto p = random_point();
            const auto i = point_to_index(p);
            active.emplace_back(0);
            samples.emplace_back(p);
            grid.Value(i.x, i.y, 0);
        }

        auto can_place_at = [&](const vec2f& sample, const vec2i& sample_index)
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
        };

        while(!active.empty())
        {
            const auto active_index = random->NextRange(active.size());
            const auto base_sample = samples[active[active_index]];

            for(int try_index = 0; try_index<k; try_index +=1)
            {
                const auto unit = RandomUnit(random);
                const auto random_range = random->NextRange(r, 2*r);
                const auto sample = base_sample + unit * random_range;
                const auto sample_index = point_to_index(sample);

                if(!grid.IsInside(sample_index.x, sample_index.y)) {continue;}

                if(can_place_at(sample, sample_index))
                {
                    const auto point_index = samples.size();

                    samples.emplace_back(sample);
                    ASSERT( grid.Value(sample_index.x, sample_index.y) == -1);
                    grid.Value(sample_index.x, sample_index.y, point_index);
                    active.emplace_back(point_index);
                }
                else
                {
                    active.erase(active.begin() + active_index);
                    break;
                }
            }
        }

        // todo(Gustav): verify sample is inside rect
        return samples;
    }
}
