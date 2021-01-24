#include <fstream>
#include <iostream>

#include "core/cypher.h"
#include "core/argparse.h"


using namespace euphoria::core;
namespace core = euphoria::core;


void RunRot13FromStream(std::istream& stream)
{
    std::string line;
    while(std::getline(stream, line))
    {
        const auto transformed = Rot13(line);
        std::cout << transformed;
    }

    std::cout << "\n";
}


void RunRot13(const std::string& input)
{
    if(input == "-")
    {
        RunRot13FromStream(std::cin);
    }
    else
    {
        std::ifstream f{input.c_str()};
        if(f.good())
        {
            RunRot13FromStream(f);
        }
        else
        {
            std::cerr << "Failed to open " << input << "\n";
        }

    }

}



int
main(int argc, char* argv[])
{
    core::argparse::Parser parser {"Cypher tool"};

    auto sub = parser.AddSubParsers();

    sub->Add
    (
        "rot13",
        "runs rot13 on input",
        [&](core::argparse::SubParser* sent)
        {
            std::string input;

            sent->Add("input", &input);

            return sent->OnComplete([&]
            {
                RunRot13(input);
                return core::argparse::ParseResult::Ok;
            });
        }
    );

    return core::argparse::ParseFromMain(&parser, argc, argv);
}
