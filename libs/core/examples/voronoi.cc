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

std::vector<vec2f> GenerateRandomPoints(int count, const rectf& size, random* random)
{
    std::vector<vec2f> r;
    for(int i=0; i<count; i+=1)
    {
        r.emplace_back( size.get_random_point(random) );
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
    return {euphoria::core::abs(a.x), euphoria::core::abs(a.y)};
}

float euclidian_distance(const vec2f& lhs, const vec2f& rhs)
{
    return (lhs-rhs).get_length_squared();
}

float manhattan_distance(const vec2f& lhs, const vec2f& rhs)
{
    const auto d = Abs(lhs-rhs);
    return d.x + d.y;
}

float min_distance(const vec2f& lhs, const vec2f& rhs)
{
    const auto d = Abs(lhs-rhs);
    return min(d.x, d.y);
}

float max_distance(const vec2f& lhs, const vec2f& rhs)
{
    const auto d = Abs(lhs-rhs);
    return max(d.x, d.y);
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
    return 0.0f;
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

    auto parser = argparse::parser {"voronoi generator"};

    // todo(Gustav): change to a subparser
    parser.add("--gen", &point_generation).set_help("How to generate points");
    parser.add("--nopoints", &number_of_points).set_help("number of points (in random)");
    parser.add("--radius", &poisson_radius).set_help("point radius (in poisson)");

    parser.add("--imsize", &size).set_help("image size");
    parser.add("--output", &output_path).set_help("where to save the result");
    parser.set_true("--colorblind", &use_colorblind).set_help("Switch to a colorblind palette");
    parser.set_true("--cos", &cos_distance).set_help("Instead of distance, cos(distance)");
    parser.add("--crazy", &crazy_distance).set_help("Sort distance acording to abs(value-distance)");
    parser.add("--distance", &distance_function).set_help("How to calculate distance");

    if(const auto r = parser.parse(argc, argv))
    {
        return *r;
    }

    random rand;

    const auto area = rectf::from_width_height(size, size);
    const auto random_points =
        point_generation == PointGeneration::Random
        ? GenerateRandomPoints(number_of_points, area, &rand)
        : poisson_sample(area, &rand, poisson_radius*2, poisson_radius);

    auto pal = use_colorblind
        ? palettes::color_blind_10()
        : palette::create_rainbow(random_points.size());
    image image;
    image.setup_no_alpha_support(size, size);

    auto points = get_closest_point<vec2f, int, std::function<float (const vec2f&, const vec2f&)>, float>
        {
            [&](const vec2f& lhs, const vec2f& rhs)
            {
                const auto dist = GetDistance(distance_function, lhs, rhs);
                if(cos_distance)
                {
                    return cos(angle::from_degrees(dist/crazy_distance));
                }
                else
                {
                    return euphoria::core::abs(crazy_distance - dist);
                }
            }
        }
        ;
    {
        int index = 0;
        for(auto p: random_points)
        {
            points.add(p, index);
            index += 1;
        }
    }

    image.set_all_bottom_top([&](int x, int y) {
        const auto index = points.find_closest(vec2f{i2f(x), i2f(y)});
        return pal.get_safe_index(index);
    });


    io::chunk_to_file(image.write(image_write_format::png), output_path);

    return 0;
}
