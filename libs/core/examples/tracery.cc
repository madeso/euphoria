#include "core/tracery.h"

#include "core/argparse.h"
#include "core/random.h"

#include <iostream>
#include <fstream>


namespace tracery = euphoria::core::tracery;
namespace argparse = euphoria::core::argparse;

tracery::Result
load_from_file(tracery::Grammar* grammar, const std::string& file)
{
    std::ifstream t(file);
    if(t.good() == false)
    {
        return tracery::Result(tracery::Result::unable_to_open_file) << file;
    }
    const auto data = std::string
    (
        std::istreambuf_iterator<char>(t),
        std::istreambuf_iterator<char>()
    );
    return grammar->load_from_string(file, data);
}

int
main(int argc, char* argv[])
{
    using namespace euphoria::core::argparse;

    std::string file;
    std::string rule = "#origin#";
    int count = 1;

    auto parser = euphoria::core::argparse::Parser{"Tracery-like terminal interface."};
    parser.add("file", &file).set_help("the tracery rule file");
    parser.add("--rule", &rule).set_help("the starting to expand from");
    parser.add("--count", &count).set_help("how many times to run the rule");

    parser.on_complete([&]
    {
        tracery::Grammar grammar;
        auto random = euphoria::core::Random{};

        grammar.register_english();

        const auto load_result = load_from_file(&grammar, file);
        if(load_result == false)
        {
            std::cerr << load_result << "\n";
            return argparse::error;
        }

        for(int index = 0; index < count; index += 1)
        {
            const auto flatten_result = grammar.flatten(&random, rule);
            if(flatten_result == false)
            {
                std::cerr << flatten_result;
                return argparse::error;
            }
            std::cout << flatten_result.get_text() << "\n";
        }

        return argparse::ok;
    }
    );
    return parse_from_main(&parser, argc, argv);
}
