#include "core/tracery.h"

#include "core/argparse.h"

#include <iostream>
#include <fstream>


namespace tracery = euphoria::core::tracery;

tracery::Result
LoadFromFile(tracery::Grammar* grammar, const std::string& file)
{
    std::ifstream t(file);
    if(t.good() == false)
    {
        return tracery::Result(tracery::Result::UNABLE_TO_OPEN_FILE) << file;
    }
    std::string data(
            (std::istreambuf_iterator<char>(t)),
            std::istreambuf_iterator<char>());
    return grammar->LoadFromString(data);
}

int
main(int argc, char* argv[])
{
    using namespace euphoria::core::argparse;

    std::string file;
    std::string rule = "#origin#";
    int count = 1;

    auto parser = Parser{"Tracery-like terminal interface."};
    parser.Add("file", &file).Help("the tracery rule file");
    parser.Add("--rule", &rule).Help("the starting to expand from");
    parser.Add("--count", &count).Help("how many times to run the rule");

    parser.OnComplete([&]
    {
        tracery::Grammar grammar;

        grammar.RegisterEnglish();

        const auto load_result = LoadFromFile(&grammar, file);
        if(load_result == false)
        {
            std::cerr << load_result << "\n";
            return ParseResult::Error;
        }

        for(int i = 0; i < count; ++i)
        {
            const auto flatten_result = grammar.Flatten(rule);
            if(flatten_result == false)
            {
                std::cerr << flatten_result;
                return ParseResult::Error;
            }
            std::cout << flatten_result.GetText() << "\n";
        }

        return ParseResult::Ok;
    }
    );
    return ParseFromMain(&parser, argc, argv);
}
