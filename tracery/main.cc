#include "core/tracery.h"
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
    // todo(Gustav): use argparse!
    if(argc >= 2)
    {
        tracery::Grammar grammar;

        const std::string file = argv[1];

        grammar.RegisterEnglish();

        const auto load_result = LoadFromFile(&grammar, file);
        if(load_result == false)
        {
            std::cerr << load_result << "\n";
            return 3;
        }

        const std::string rule  = (argc >= 3) ? argv[2] : "#origin#";
        const int         count = (argc >= 4) ? atoi(argv[3]) : 1;
        for(int i = 0; i < count; ++i)
        {
            const auto flatten_result = grammar.Flatten(rule);
            if(flatten_result == false)
            {
                std::cerr << flatten_result;
                return 2;
            }
            std::cout << flatten_result.GetText() << "\n";
        }
    }
    else
    {
        std::cerr << "Expected input!\n";
        std::cerr << argv[0] << " json-file [rule] [count]\n";
    }
    return 0;
}
