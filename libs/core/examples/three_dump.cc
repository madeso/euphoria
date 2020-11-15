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

void AddSpiral(Dumper& dump, int resolution, float number_of_spins, float height, float size, const Rgbi& color)
{
    std::vector<vec3f> points;
    for(int i=0; i<resolution; i+=1)
    {
        const auto angle = Angle::FromPercentOf360(i*number_of_spins / resolution);
        points.emplace_back(Sin(angle)*size, i*height/resolution, Cos(angle)*size);
    }
    dump.AddLines(points, color);
}

int
main(int, char*[])
{
    Random rand;

    auto pal = CreateShuffleBag(palette::ColorBlind_10().colors, 2);

    const auto size = 15.0f;
    auto area = Aabb{vec3f{-size, -size, -size}, vec3f{size, size, size}};
    auto dump = Dumper{"three.html"};

    for(int i=0; i<30; i+=1)
    {
        dump.AddSphere(area.RandomPoint(&rand), 1.0f, pal.Next(&rand));
    }

    AddSpiral(dump, 100, 4, 5, 2.5, pal.Next(&rand));

    dump.AddPlane(Plane::FromNormalAndPoint(unit3f::Right(), vec3f(1, 2, 3)), pal.Next(&rand));

    dump.AddArrow(Ray3f::FromTo(vec3f{3, 2, 1}, vec3f{0,0,0}), pal.Next(&rand));

    dump.AddAxis();
    dump.AddGrid();

    // dump.AddAxis();
}

