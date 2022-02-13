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
#include "core/cint.h"

#include "core/palette_tableu.h"
#include "core/poisson.h"

using namespace euphoria::core;
namespace euco = euphoria::core;

float
i2f(int i)
{
    return static_cast<float>(i);
}

std::vector<Vec2f> generate_random_points(int count, const Rectf& size, euco::Random* random)
{
    std::vector<Vec2f> r;
    for(int i=0; i<count; i+=1)
    {
        r.emplace_back( get_random_point(random, size) );
    }
    return r;
}

enum class DistanceFunction
{
    euclidian, manhattan, min, max
};

enum class PointGeneration
{
    random, poisson
};

Vec2f abs(const Vec2f& a)
{
    return {euphoria::core::abs(a.x), euphoria::core::abs(a.y)};
}

float euclidian_distance(const Vec2f& lhs, const Vec2f& rhs)
{
    return (lhs-rhs).get_length_squared();
}

float manhattan_distance(const Vec2f& lhs, const Vec2f& rhs)
{
    const auto d = abs(lhs-rhs);
    return d.x + d.y;
}

float min_distance(const Vec2f& lhs, const Vec2f& rhs)
{
    const auto d = abs(lhs-rhs);
    return min(d.x, d.y);
}

float max_distance(const Vec2f& lhs, const Vec2f& rhs)
{
    const auto d = abs(lhs-rhs);
    return max(d.x, d.y);
}

float get_distance(DistanceFunction f, const Vec2f& lhs, const Vec2f& rhs)
{
    switch(f)
    {
        case DistanceFunction::euclidian: return euclidian_distance(lhs, rhs);
        case DistanceFunction::manhattan: return manhattan_distance(lhs, rhs);
        case DistanceFunction::min: return min_distance(lhs, rhs);
        case DistanceFunction::max: return max_distance(lhs, rhs);
    }
    DIE("Unhandled distancfe function");
    return 0.0f;
}


int
main(int argc, char* argv[])
{
    auto size = 512;

    auto distance_function = DistanceFunction::euclidian;

    auto point_generation = PointGeneration::random;
    auto number_of_points = 30;
    auto poisson_radius = 10.0f;
    auto use_colorblind = false;
    auto cos_distance = false;
    auto crazy_distance = 0.0f;
    std::string output_path = "voronoi.png";

    auto parser = argparse::Parser {"voronoi generator"};

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

    euco::Random rand;

    const auto area = Rectf::from_width_height(c_sizet_to_float(size), c_sizet_to_float(size));
    const auto random_points =
        point_generation == PointGeneration::random
        ? generate_random_points(number_of_points, area, &rand)
        : poisson_sample(area, &rand, poisson_radius*2, poisson_radius);

    const auto rainbow = DynamicPalette::create_rainbow(c_sizet_to_int(random_points.size()));
    auto pal = use_colorblind
        ? *palettes::tableau::color_blind_10
        : rainbow.to_palette();
    Image image;
    image.setup_no_alpha_support(size, size);

    auto points = ClosestPointCollector<Vec2f, int, std::function<float (const Vec2f&, const Vec2f&)>, float>
        {
            [&](const Vec2f& lhs, const Vec2f& rhs)
            {
                const auto dist = get_distance(distance_function, lhs, rhs);
                if(cos_distance)
                {
                    return cos(Angle::from_degrees(dist/crazy_distance));
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
        const auto index = points.find_closest(Vec2f{i2f(x), i2f(y)});
        return pal.get_safe_index(index);
    });


    io::chunk_to_file(image.write(ImageWriteFormat::png), output_path);

    return 0;
}
