#include <fstream>
#include <iostream>

#include "core/cypher.h"
#include "core/argparse.h"


using namespace eu;
using namespace eu::core;
namespace core = eu::core;


void run_rot13_from_stream(std::istream& stream)
{
    std::string line;
    while(std::getline(stream, line))
    {
        const auto transformed = get_rot13(line);
        std::cout << transformed;
    }

    std::cout << "\n";
}


void run_rot13(const std::string& input)
{
    if(input == "-")
    {
        run_rot13_from_stream(std::cin);
    }
    else
    {
        std::ifstream f{input.c_str()};
        if(f.good())
        {
            run_rot13_from_stream(f);
        }
        else
        {
            std::istringstream s(input);
            run_rot13_from_stream(s);
        }

    }

}


int
main(int argc, char* argv[])
{
    core::argparse::Parser parser {"Cypher tool"};

    auto sub = parser.add_sub_parsers();

    sub->add
    (
        "get_rot13",
        "runs get_rot13 on input",
        [&](core::argparse::SubParser* sent)
        {
            std::string input;

            sent->add("input", &input)
                .set_help("path to a file (- for stdin) or just text");

            return sent->on_complete([&]
            {
                run_rot13(input);
                return core::argparse::ok;
            });
        }
    );

    return core::argparse::parse_from_main(&parser, argc, argv);
}
