#include <iostream>
#include <functional>

#include "core/image.h"
#include "core/random.h"
#include "core/io.h"
#include "core/vec2.h"
#include "core/palette.h"
#include "core/closestpoint.h"
#include "core/rect.h"

#include "core/palette_tableu.h"
#include "core/poisson.h"

using namespace euphoria::core;

float
i2f(int i)
{
    return static_cast<float>(i);
}

std::vector<vec2f> GenerateRandomPoints(int count, const Rectf& size, Random* random)
{
    std::vector<vec2f> r;
    for(int i=0; i<count; i+=1)
    {
        r.emplace_back( size.RandomPoint(random) );
    }
    return r;
}

int
main(int argc, char* argv[])
{
    const auto size = 512;
    // const auto number_of_points = 30;

    Random rand;

    const auto area = Rectf::FromWidthHeight(size, size);
    const auto random_points = GenerateRandomPoints(30, area, &rand);
    // const auto random_points = PoissonSample(area, &rand, 10.0f);


    using DistFunc = std::function<float(const vec2f&, const vec2f&)>;

    DistFunc euclidian_distance = [](const vec2f& lhs, const vec2f& rhs)
    {
        return (lhs-rhs).GetLengthSquared();
    };

    DistFunc manhattan_distance = [](const vec2f& lhs, const vec2f& rhs)
    {
        const auto d = (lhs-rhs);
        return Abs(d.x) + Abs(d.y);
    };
    
    auto pal = Palette::Rainbow(random_points.size());
    // auto pal = palette::ColorBlind_10();
    Image image;
    image.SetupNoAlphaSupport(size, size);

    ClosestPoint<vec2f, int, DistFunc, float> points(euclidian_distance);
    {
        int index = 0;
        for(auto p: random_points)
        {
            points.Add(p, index);
            index += 1;
        }
    }

    image.SetAllBottomTop([&](int x, int y) {
        const auto index = points.FindClosest(vec2f{i2f(x), i2f(y)});
        return pal.GetSafeIndex(index);
    });


    io::ChunkToFile(image.Write(ImageWriteFormat::PNG), "voronoi.png");
}
