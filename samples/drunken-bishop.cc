#include "core/argparse.h"

#include "core/drunken_bishop.h"

#include "core/random.h"
#include "core/imageops.h"

#include "core/io.h"

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace euphoria::core;


int
main(int argc, char* argv[])
{
    auto parser = argparse::Parser {"Drunken bishops"};

    int width = 17;
    int height = 9;

    auto add_common = [&](argparse::Parser& parser)
    {
        parser.AddSimple("--width", &width).Help("set the height");
        parser.AddSimple("--height", &height).Help("set the width");
    };

    auto precursive = parser.AddSubParser
    (
        "text",
        "drunken bishop with ssh like output",
        [&]
        {
            auto random = Random{};
            const auto hash = random.NextInteger64();
            const auto table = DrunkenBishop(hash, width, height);
            const auto str = Collapse(table, GetSshCharacters());
            std::cout << str << "\n";
        }
    );
    add_common(*precursive);

    const auto status = parser.Parse(argc, argv);
    if(status != argparse::ParseResult::Ok)
    {
        return -1;
    }

    return 0;
}
