#include <iostream>
#include <functional>

#include "core/argparse.h"
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

enum class DistanceFunction
{
    Euclidian, Manhattan
};
DECLARE_ENUM_LIST(DistanceFunction)
BEGIN_ENUM_LIST(DistanceFunction)
    ENUM_VALUE(DistanceFunction, Euclidian)
    ENUM_VALUE(DistanceFunction, Manhattan)
END_ENUM_LIST()

enum class PointGeneration
{
    Random, Poisson
};
DECLARE_ENUM_LIST(PointGeneration)
BEGIN_ENUM_LIST(PointGeneration)
    ENUM_VALUE(PointGeneration, Random)
    ENUM_VALUE(PointGeneration, Poisson)
END_ENUM_LIST()

int
main(int argc, char* argv[])
{
    auto size = 512;

    auto distance_function = DistanceFunction::Euclidian;

    auto point_generation = PointGeneration::Random;
    auto number_of_points = 30;
    auto poisson_radius = 10.0f;
    auto use_colorblind = false;
    std::string output_path = "voronoi.png";

    auto parser = argparse::Parser {"voronoi generator"};

    parser.AddSimple("-nopoints", &number_of_points).Help("number of points (in random)");
    parser.AddSimple("-radius", &poisson_radius).Help("point radius (in poisson)");
    parser.AddSimple("-imsize", &size).Help("image size");
    parser.AddSimple("-output", &output_path).Help("where to save the result");
    parser.SetTrue("-colorblind", &use_colorblind).Help("Switch to a colorblind palette");
    parser.AddEnum("-distance", &distance_function).Help("How to calculate distance");
    parser.AddEnum("-gen", &point_generation).Help("How to generate points");

    if(parser.Parse(argc, argv) != argparse::ParseResult::Ok) { return -1; }

    Random rand;

    const auto area = Rectf::FromWidthHeight(size, size);
    const auto random_points =
        point_generation == PointGeneration::Random
        ? GenerateRandomPoints(number_of_points, area, &rand)
        : PoissonSample(area, &rand, poisson_radius*2, poisson_radius);


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
    
    auto pal = use_colorblind
        ? palette::ColorBlind_10()
        : Palette::Rainbow(random_points.size());
    Image image;
    image.SetupNoAlphaSupport(size, size);

    ClosestPoint<vec2f, int, DistFunc, float> points
        (
            distance_function == DistanceFunction::Euclidian
            ? euclidian_distance
            : manhattan_distance
        )
        ;
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


    io::ChunkToFile(image.Write(ImageWriteFormat::PNG), output_path);

    return 0;
}
