#include <iostream>
#include <functional>

#include "core/image.h"
#include "core/random.h"
#include "core/io.h"
#include "core/vec2.h"
#include "core/palette.h"
#include "core/closestpoint.h"

#include "core/palette_tableu.h"

using namespace euphoria::core;

float
i2f(int i)
{
    return static_cast<float>(i);
}

int
main(int argc, char* argv[])
{
    const auto size = 512;
    const auto number_of_points = 30;

    Random rand;

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
    
    // auto pal = Palette::Rainbow(number_of_points);
    auto pal = palette::ColorBlind_10();
    Image image;
    image.SetupNoAlphaSupport(size, size);

    ClosestPoint<vec2f, int, DistFunc, float> points(euclidian_distance);
    for(int index=0; index<number_of_points; index+=1)
    {
        const auto x = rand.NextRange(image.GetWidth());
        const auto y = rand.NextRange(image.GetHeight());
        points.Add(vec2f{i2f(x), i2f(y)}, index);
    }

    image.SetAllBottomTop([&](int x, int y) {
        const auto index = points.FindClosest(vec2f{i2f(x), i2f(y)});
        return pal.GetSafeIndex(index);
    });


    io::ChunkToFile(image.Write(ImageWriteFormat::PNG), "voronoi.png");
}
