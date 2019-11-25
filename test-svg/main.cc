#include "core/svg.h"

#include "core/numeric.h"
#include "core/angle.h"
#include "core/random.h"
#include "core/range.h"
#include "core/palette.h"
#include "core/palette_tableu.h"
#include "core/shufflebag.h"

using namespace euphoria::core;
using namespace euphoria::core::svg;

Poly
MakeStar(const vec2f& origo, float radius, const Rgbi& fill, const Angle& rotation)
{
    auto alpha = Angle::OneTurn() / 10;

    auto p = Poly{};

    for(auto i = 11; i > 0; i--)
    {
        auto r = radius*(i % 2 + 1)/2.0f;
        auto omega = rotation + alpha * static_cast<float>(i);
        p.points.push_back({(r * Sin(omega)) + origo.x, (r * Cos(omega)) + origo.y});
    }

    p.Close(fill);

    return p;
}

int
main(int, char*[])
{
    Random rand;

    auto pal = CreateShuffleBag(palette::ColorBlind_10().colors, 2);

    auto svg = Svg{};

    for(int i=0; i<30; i+=1)
    {
        const auto p = rand.PointOnUnitCircle_CenterFocused()*100.0f;
        const auto s = rand.Next(Range{5.0f, 30.0f});
        const auto c = pal.Next(&rand);
        const auto r = Angle::FromPercentOf360(rand.NextFloat01());
        svg << MakeStar(p, s, c, r);
    }
    svg.AddAxis().Grid(10);
    svg.Write("stars.html");
}

