#include "core/dump.h"

#include <iomanip>

#include "core/numeric.h"
#include "core/angle.h"
#include "core/random.h"
#include "core/range.h"
#include "core/palette.h"
#include "core/palette_tableu.h"
#include "core/shufflebag.h"
#include "core/str.h"

using namespace euphoria::core;
using namespace euphoria::core::dump2d;

poly
MakeStar(const vec2f& origo, float radius, const angle& rotation, int number_of_points=5, float inner_scale=0.5f)
{
    auto angle_step = angle::OneTurn() / (static_cast<float>(number_of_points)*2.0f);

    auto poly = ::poly{};

    for(auto i=0; i<(number_of_points*2); i+=1)
    {
        const auto is_even = i % 2 == 0;
        auto r =  is_even ? radius*inner_scale : radius;
        auto point_rotation = rotation + angle_step * static_cast<float>(i);
        const auto x = (r * sin(point_rotation)) + origo.x;
        const auto y = (r * cos(point_rotation)) + origo.y;
        poly.points.emplace_back(x, y);
    }

    return poly;
}

int
main(int, char*[])
{
    Random rand;

    auto pal = CreateShuffleBag(palettes::color_blind_10().colors, 2);

    auto svg = dumper{};

    for(int i=0; i<30; i+=1)
    {
        const auto center = rand.PointOnUnitCircle_CenterFocused()*200.0f;
        const auto radius = rand.Next(Range{3.0f, 30.0f});
        const auto p = rand.Next(Range{4, 10});
        const auto inner = rand.Next(Range{0.3f, 0.75f});
        const auto fill = pal.Next(&rand);
        const auto rotation = angle::from_percent_of_360(rand.NextFloat01());
        svg << MakeStar(center, radius, rotation, p, inner).close().fill(fill)
            // << Text(center, Str{} << p << " / " << std::setprecision( 2 ) << inner)
            ;
    }
    svg.add_axis();
    svg.write("stars.html");
}

