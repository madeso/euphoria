#include "core/dump.h"

#include <iomanip>

#include "core/numeric.h"
#include "core/angle.h"
#include "core/random.h"
#include "core/range.h"
#include "core/palette.h"
#include "core/palette_tableu.h"
#include "core/shufflebag.h"
#include "base/os.h"
#include "core/argparse.h"


using namespace eu;
using namespace eu::core;
using namespace eu::core::dump2d;


Poly
make_star(const vec2f& origo, float radius, const Angle& rotation, int number_of_points=5, float inner_scale=0.5f)
{
    auto angle_step = one_turn / (static_cast<float>(number_of_points)*2.0f);

    auto poly = ::Poly{};

    for(auto index=0; index<(number_of_points*2); index += 1)
    {
        const auto is_even = index % 2 == 0;
        auto r = is_even ? radius*inner_scale : radius;
        auto point_rotation = rotation + angle_step * static_cast<float>(index);
        const auto x = (r * sin(point_rotation)) + origo.x;
        const auto y = (r * cos(point_rotation)) + origo.y;
        poly.points.emplace_back(x, y);
    }

    return poly;
}


int
main(int argc, char* argv[])
{
    auto parser = argparse::Parser("svg dump");

    auto current_directory = get_current_directory();
    parser
        .add("-w", &current_directory)
        .set_help("Sets the working direction if it's different from the current folder")
        ;
    const auto parse_result = parser.parse(argparse::NameAndArguments::extract(argc, argv));
    if(parse_result != core::argparse::ok)
    {
        return parse_result.return_value;
    }

    core::Random rand;

    auto pal = create_shuffle_bag(palettes::tableau::color_blind_10->colors, 2);

    auto svg = Dumper{};

    for(int star_index=0; star_index<30; star_index += 1)
    {
        const auto center = get_random_point_on_unit_circle_center_focused(&rand)*200.0f;
        const auto radius = get_random_in_range(&rand, Range{3.0f, 30.0f});
        const auto points = get_random_in_range(&rand, Range{4, 10});
        const auto inner = get_random_in_range(&rand, Range{0.3f, 0.75f});
        const auto fill = pal.get_random_item(&rand);
        const auto rotation = Angle::from_percent_of_360(rand.get_next_float01());
        svg << make_star(center, radius, rotation, points, inner).close().fill(fill)
            // << Text(center, Str{} << p << " / " << std::setprecision( 2 ) << inner)
            ;
    }
    svg.add_axis();
    svg.write(current_directory + "/stars.html");

    return 0;
}

