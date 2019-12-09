#include "core/argparse.h"
#include "core/dump.h"
#include "core/poisson.h"
#include "core/random.h"
#include "core/rect.h"

using namespace euphoria::core;
using namespace euphoria::core::dump2d;

void
svg_dump()
{
    Random random;

    const auto area = Rectf::FromWidthHeight(100, 100);
    const auto radius = 5.0f;

    const auto points = PoissonSample(area, &random, radius);

    auto svg = Dumper{};

    for(auto p: points)
    {
        svg << Circle{p, radius}.Line(Color::Black);
    }
    svg.AddAxis();
    svg.Write("poisson.html", 800, 600);
}

int
main(int argc, char* argv[])
{
    auto parser = argparse::Parser {"Poisson test"};

    auto pcell = parser.AddSubParser(
            "svg", "generate svg file", [&] {
                svg_dump();
            });

    const auto status = parser.Parse(argc, argv);
    if(status != argparse::ParseResult::Ok)
    {
        return -1;
    }


    return 0;
}

