#include "core/dump.h"

#include "core/random.h"
#include "core/palette.h"
#include "core/angle.h"
#include "core/palette_tableu.h"
#include "core/shufflebag.h"
#include "core/plane.h"
#include "core/ray.h"
#include "core/os.h"
#include "core/argparse.h"

#include "core/aabb.h"

using namespace euphoria::core;
using namespace euphoria::core::dump3d;

void add_spiral(Dumper* dump, int number_of_steps, float number_of_spins, float height, float size, const Rgbi& color)
{
    std::vector<Vec3f> points;
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

    euphoria::core::Random rand;

    auto pal = create_shuffle_bag(palettes::tableau::color_blind_10->colors, 2);

    const auto size = 15.0f;
    auto area = Aabb{Vec3f{-size, -size, -size}, Vec3f{size, size, size}};
    auto dump = Dumper{current_directory + "/three.html"};

    for(int i=0; i<30; i+=1)
    {
        dump.add_sphere(get_random_point(&rand, area), 1.0f, pal.get_random_item(&rand));
    }

    add_spiral(&dump, 100, 4, 5, 2.5f, pal.get_random_item(&rand));

    dump.add_plane(Plane::from_normal_and_point(Unit3f::right(), Vec3f(1, 2, 3)), pal.get_random_item(&rand));

    dump.add_arrow(Ray3f::from_to(Vec3f{3, 2, 1}, Vec3f{0,0,0}), pal.get_random_item(&rand));

    dump.add_axis();
    dump.add_grid();

    // dump.AddAxis();

    return 0;
}

