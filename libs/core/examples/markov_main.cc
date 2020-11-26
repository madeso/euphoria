#include <string>
#include <iostream>
#include <fstream>
#include <set>

#include "core/markov.h"
#include "core/argparse.h"
#include "core/stringmerger.h"
#include "core/stringutils.h"
#include "core/editdistance.h"
#include "core/nlp_sentance.h"
#include "core/nlp_line.h"

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


struct Similar
{
    virtual ~Similar() = default;
    virtual void Add(const std::string& line) = 0;
    virtual bool IsSame(const std::string& generated) const = 0;
};


struct SimilarSet : public Similar
{
    std::set<std::string> existing_lines;

    void Add(const std::string& line) override
    {
        existing_lines.emplace(line);
    }

    bool IsSame(const std::string& generated) const override
    {
        return existing_lines.find(generated) != existing_lines.end();
    }

    static std::unique_ptr<Similar> Create()
    {
         return std::make_unique<SimilarSet>();
    }
};


struct SimilarEditDistance : public Similar
{
    std::vector<std::string> existing_lines;

    void Add(const std::string& line) override
    {
        existing_lines.emplace_back(line);
    }

    bool IsSame(const std::string& generated) const override
    {
        const auto count = generated.size();

        for(const auto& line: existing_lines)
        {
            const auto shortened = core::FirstChars(line, count);
            const int edits = core::FastEditDistance(shortened, generated);
            if(edits < 3)
            {
                return true;
            }
        }
        return false;
    }

    static std::unique_ptr<Similar> Create()
    {
         return std::make_unique<SimilarEditDistance>();
    }
};


void
MarkovLine(const std::string& file, int memory, int count, bool also_existing, bool simple)
{
    core::Random rnd;
    markov::ChainBuilder<std::string> m {memory};

    std::ifstream data;
    data.open(file);
    if(!data)
    {
        std::cerr << "Failed to open input\n";
        return;
    }

    std::unique_ptr<Similar> existing_lines = simple ? SimilarSet::Create() : SimilarEditDistance::Create();

    {
        std::string line;
        while(std::getline(data, line))
        {
            if(line.empty()) { continue; }
            const auto p = core::ParseLine(line);
            if(!p) { continue; }

            if(!also_existing)
            {
                existing_lines->Add(core::ToLower(line));
            }
            m.Add(*p);
        }
    }

    std::cout << "\n";
    auto b = m.Build();

    int skipped = 0;

    for(int i = 0; i < count+skipped; i += 1)
    {
        const auto generated = core::LineToString(b.Generate(&rnd));
        if(!also_existing && existing_lines->IsSame(core::ToLower(generated)))
        {
            skipped += 1;

            if(skipped >= 5000)
            {
                break;
            }
        }
        else
        {
            std::cout << generated << "\n";
        }
    }

    if(!also_existing)
    {
        std::cout << "Items skipped: " << skipped << "\n";
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
    
    sub->Add
    (
        "line",
        "parses and generates lines",
        [&](core::argparse::SubParser* line_parser)
        {
            std::string file;
            int memory = 2;
            int count = 25;
            bool existing = false;
            bool simple = false;

            line_parser->Add("file", &file);
            line_parser->Add("--memory", &memory);
            line_parser->Add("--count", &count);
            line_parser->Add("--existing", &existing);
            line_parser->Add("--simple", &simple);

            return line_parser->OnComplete([&]
            {
                MarkovLine(file, memory, count, existing, simple);
                return core::argparse::ParseResult::Ok;
            });
        }
    );
    

    return core::argparse::ParseFromMain(&parser, argc, argv);
}
