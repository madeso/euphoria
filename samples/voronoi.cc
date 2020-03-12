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
    Euclidian, Manhattan, Min, Max
};

enum class PointGeneration
{
    Random, Poisson
};

vec2f Abs(const vec2f& a)
{
    return {Abs(a.x), Abs(a.y)};
}

float euclidian_distance(const vec2f& lhs, const vec2f& rhs)
{
    return (lhs-rhs).GetLengthSquared();
}

float manhattan_distance(const vec2f& lhs, const vec2f& rhs)
{
    const auto d = Abs(lhs-rhs);
    return d.x + d.y;
}

float min_distance(const vec2f& lhs, const vec2f& rhs)
{
    const auto d = Abs(lhs-rhs);
    return Min(d.x, d.y);
}

float max_distance(const vec2f& lhs, const vec2f& rhs)
{
    const auto d = Abs(lhs-rhs);
    return Max(d.x, d.y);
}

float GetDistance(DistanceFunction f, const vec2f& lhs, const vec2f& rhs)
{
    switch(f)
    {
        case DistanceFunction::Euclidian: return euclidian_distance(lhs, rhs);
        case DistanceFunction::Manhattan: return manhattan_distance(lhs, rhs);
        case DistanceFunction::Min: return min_distance(lhs, rhs);
        case DistanceFunction::Max: return max_distance(lhs, rhs);
    }
    DIE("Unhandled distancfe function");
};


int
main(int argc, char* argv[])
{
    auto size = 512;

    auto distance_function = DistanceFunction::Euclidian;

    auto point_generation = PointGeneration::Random;
    auto number_of_points = 30;
    auto poisson_radius = 10.0f;
    auto use_colorblind = false;
    auto cos_distance = false;
    auto crazy_distance = 0.0f;
    std::string output_path = "voronoi.png";

    auto parser = argparse::Parser {"voronoi generator"};

    // todo(Gustav): change to a subparser
    parser.Add("--gen", &point_generation).Help("How to generate points");
    parser.Add("--nopoints", &number_of_points).Help("number of points (in random)");
    parser.Add("--radius", &poisson_radius).Help("point radius (in poisson)");

    parser.Add("--imsize", &size).Help("image size");
    parser.Add("--output", &output_path).Help("where to save the result");
    parser.SetTrue("--colorblind", &use_colorblind).Help("Switch to a colorblind palette");
    parser.SetTrue("--cos", &cos_distance).Help("Instead of distance, cos(distance)");
    parser.Add("--crazy", &crazy_distance).Help("Sort distance acording to abs(value-distance)");
    parser.Add("--distance", &distance_function).Help("How to calculate distance");

    if(const auto r = parser.Parse(argc, argv))
    {
        return *r;
    }

    Random rand;

    const auto area = Rectf::FromWidthHeight(size, size);
    const auto random_points =
        point_generation == PointGeneration::Random
        ? GenerateRandomPoints(number_of_points, area, &rand)
        : PoissonSample(area, &rand, poisson_radius*2, poisson_radius);

    auto pal = use_colorblind
        ? palette::ColorBlind_10()
        : Palette::Rainbow(random_points.size());
    Image image;
    image.SetupNoAlphaSupport(size, size);

    auto points = ClosestPoint<vec2f, int, std::function<float (const vec2f&, const vec2f&)>, float>
        {
            [&](const vec2f& lhs, const vec2f& rhs)
            {
                const auto dist = GetDistance(distance_function, lhs, rhs);
                if(cos_distance)
                {
                    return Cos(Angle::FromDegrees(dist/crazy_distance));
                }
                else
                {
                    return Abs(crazy_distance - dist);
                }
            }
        }
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
