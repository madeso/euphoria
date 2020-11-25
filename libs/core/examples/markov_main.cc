#include <string>
#include <iostream>
#include <fstream>

#include "core/markov.h"
#include "core/argparse.h"
#include "core/stringmerger.h"
#include "core/nlp_sentance.h"

namespace core = euphoria::core;
namespace markov = euphoria::core::markov;


std::vector<char>
C(const std::string& str)
{
    return std::vector<char> {str.begin(), str.end()};
}


std::string
C(const std::vector<char>& v)
{
    return std::string {v.begin(), v.end()};
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void
MarkovSentance(const std::string& file, int memory, int count)
{
    std::ifstream data;
    data.open(file);
    if(!data)
    {
        std::cerr << "Failed to open input\n";
        return;
    }

    markov::ChainBuilder<std::string> m {memory};
    const auto parsed = core::ParseSentances
    (
        data,
        [&](const core::Sentance& s) { m.Add(s); }
    );
    if(!parsed)
    {
        std::cerr << "No sentances loaded\n";
        return;
    }

    core::Random rnd;
    auto b = m.Build();

    for(int i = 0; i < count; i += 1)
    {
        auto s = b.Generate(&rnd);
        std::cout << core::SentanceToString(s) << "\n\n";
    }
}


void
MarkovWord(const std::string& file, int memory, int count)
{
    core::Random rnd;
    markov::ChainBuilder<char> m {memory};

    std::ifstream data;
    data.open(file);
    if(!data)
    {
        std::cerr << "Failed to open input\n";
        return;
    }

    std::string line;
    while(std::getline(data, line))
    {
        if(line.empty())
            continue;
        m.Add(C(line));
    }

    std::cout << "\n";
    auto b = m.Build();

    for(int i = 0; i < count; i += 1)
    {
        std::cout << C(b.Generate(&rnd)) << "\n";
    }
}


int
main(int argc, char* argv[])
{
    core::argparse::Parser parser {"markov tool"};

    auto sub = parser.AddSubParsers();

    sub->Add
    (
        "sentance",
        "parses and generates sentances",
        [&](core::argparse::SubParser* sent)
        {
            std::string file;
            int memory = 4;
            int count = 25;

            sent->Add("file", &file);
            sent->Add("--memory", &memory);
            sent->Add("--count", &count);

            return sent->OnComplete([&]
            {
                MarkovSentance(file, memory, count);
                return core::argparse::ParseResult::Ok;
            });
        }
    );

    sub->Add
    (
        "word",
        "parses and generates word",
        [&](core::argparse::SubParser* word)
        {
            std::string file;
            int memory = 4;
            int count = 25;

            word->Add("file", &file);
            word->Add("--memory", &memory);
            word->Add("--count", &count);

            return word->OnComplete([&]
            {
                MarkovWord(file, memory, count);
                return core::argparse::ParseResult::Ok;
            });
        }
    );
    

    return core::argparse::ParseFromMain(&parser, argc, argv);
}
