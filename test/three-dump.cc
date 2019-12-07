#include "core/dump.h"

#include "core/random.h"
#include "core/palette.h"
#include "core/palette_tableu.h"
#include "core/shufflebag.h"

#include "core/aabb.h"

using namespace euphoria::core;
using namespace euphoria::core::dump3d;

int
main(int, char*[])
{
    Random rand;

    auto pal = CreateShuffleBag(palette::ColorBlind_10().colors, 2);

    const auto size = 15;
    auto area = Aabb{vec3f{-size, -size, -size}, vec3f{size, size, size}};
    auto dump = Dumper{"three.html"};

    for(int i=0; i<30; i+=1)
    {
        dump.AddSphere(area.RandomPoint(&rand), 1.0f, pal.Next(&rand));
    }

    // dump.AddAxis();
}

