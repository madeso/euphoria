#include "core/dump.h"

#include "base/random.h"
#include "core/palette.h"
#include "base/angle.h"
#include "core/palette.h"
#include "core/shufflebag.h"
#include "base/plane.h"
#include "base/ray.h"
#include "base/os.h"
#include "core/argparse.h"

#include "base/aabb.h"

using namespace eu;
using namespace eu::core;
using namespace eu::core::dump3d;


void add_spiral(Dumper* dump, int number_of_steps, float number_of_spins, float height, float size, const Rgbi& color)
{
    std::vector<vec3f> points;
    for(int step_index=0; step_index<number_of_steps; step_index+=1)
    {
        const auto step = static_cast<float>(step_index) / static_cast<float>(number_of_steps);
        const auto angle = Angle::from_percent_of_360(step * number_of_spins);
        points.emplace_back(sin(angle)*size, step*height, cos(angle)*size);
    }
    dump->add_lines(points, color);
}

int
main(int argc, char* argv[])
{

    auto parser = argparse::Parser("three dump");

    auto current_directory = get_current_directory();
    parser
        .add("-w", &current_directory)
        .set_help("Sets the working direction if it's different from the current folder")
        ;
    const auto parse_result = parser.parse(argparse::NameAndArguments::extract(argc, argv));
    if(parse_result != argparse::ok)
    {
        return parse_result.return_value;
    }

    Random rand;

    auto pal = create_shuffle_bag(dawnbringer_palette->colors, 2);

    const auto size = 15.0f;
    auto area = Aabb{vec3f{-size, -size, -size}, vec3f{size, size, size}};
    auto dump = Dumper{current_directory + "/three.html"};

    for(int sphere_index=0; sphere_index<30; sphere_index+=1)
    {
        dump.add_sphere(get_random_point(&rand, area), 1.0f, pal.get_random_item(&rand));
    }

    add_spiral(&dump, 100, 4, 5, 2.5f, pal.get_random_item(&rand));

    dump.add_plane(Plane::from_normal_and_point(common::right, vec3f(1, 2, 3)), pal.get_random_item(&rand));

    dump.add_arrow(Ray3f::from_to(vec3f{3, 2, 1}, vec3f{0,0,0}), pal.get_random_item(&rand));

    dump.add_axis();
    dump.add_grid();

    // dump.AddAxis();

    return 0;
}

