#include "core/dump.h"

#include "core/random.h"
#include "core/palette.h"
#include "core/angle.h"
#include "core/palette_tableu.h"
#include "core/shufflebag.h"
#include "core/plane.h"
#include "core/ray.h"

#include "core/aabb.h"

using namespace euphoria::core;
using namespace euphoria::core::dump3d;

void AddSpiral(Dumper* dump, int number_of_steps, float number_of_spins, float height, float size, const rgbi& color)
{
    std::vector<vec3f> points;
    for(int step_index=0; step_index<number_of_steps; step_index+=1)
    {
        const auto step = static_cast<float>(step_index) / static_cast<float>(number_of_steps);
        const auto angle = angle::from_percent_of_360(step * number_of_spins);
        points.emplace_back(sin(angle)*size, step*height, cos(angle)*size);
    }
    dump->AddLines(points, color);
}

int
main(int, char*[])
{
    Random rand;

    auto pal = CreateShuffleBag(palettes::ColorBlind_10().colors, 2);

    const auto size = 15.0f;
    auto area = aabb{vec3f{-size, -size, -size}, vec3f{size, size, size}};
    auto dump = Dumper{"three.html"};

    for(int i=0; i<30; i+=1)
    {
        dump.AddSphere(area.get_random_point(&rand), 1.0f, pal.Next(&rand));
    }

    AddSpiral(&dump, 100, 4, 5, 2.5f, pal.Next(&rand));

    dump.AddPlane(plane::from_normal_and_point(unit3f::Right(), vec3f(1, 2, 3)), pal.Next(&rand));

    dump.AddArrow(ray3f::from_to(vec3f{3, 2, 1}, vec3f{0,0,0}), pal.Next(&rand));

    dump.AddAxis();
    dump.AddGrid();

    // dump.AddAxis();
}

