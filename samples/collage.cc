#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#include "core/argparse.h"

using namespace euphoria::core;

int
main(int argc, char* argv[])
{
    std::string output = "collage.png";

    // todo: input more than one file
    std::vector<std::string> files;

    auto parser = argparse::Parser {"collage tool"};

    parser.Add("-o, --output", &output);
    parser.AddVector("files", &files).Nargs("F");
    parser.OnComplete([&]{
        for(const auto& f: files)
        {
            std::cout << f << "\n";
        }
        return argparse::ParseResult::Ok;
    });

    return ParseFromMain(&parser, argc, argv);
}
