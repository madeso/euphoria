#include <fstream>
#include <sstream>

#include "core/argparse.h"

using namespace euphoria::core;

int
main(int argc, char* argv[])
{
    std::string output = "collage.png";

    // todo: input more than one file
    std::string file;

    auto parser = argparse::Parser {"collage tool"};

    parser.Add("-o, --output", &output);
    parser.Add("files", &file);

    return ParseFromMain(&parser, argc, argv);
}
