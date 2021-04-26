#include <string>
#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>

#include "core/markov.h"
#include "core/noncopyable.h"
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

    markov::chain_builder<std::string> m {memory};
    const auto parsed = core::ParseSentances
    (
        data,
        [&](const core::Sentance& s) { m.add(s); }
    );
    if(!parsed)
    {
        std::cerr << "No sentances loaded\n";
        return;
    }

    core::Random rnd;
    auto b = m.build();

    for(int i = 0; i < count; i += 1)
    {
        auto s = b.generate(&rnd);
        std::cout << core::SentanceToString(s) << "\n\n";
    }
}


void
MarkovWord(const std::string& file, int memory, int count)
{
    core::Random rnd;
    markov::chain_builder<char> m {memory};

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
        if(line.empty()) { continue; }
        m.add(C(line));
    }

    std::cout << "\n";
    auto b = m.build();

    for(int i = 0; i < count; i += 1)
    {
        std::cout << C(b.generate(&rnd)) << "\n";
    }
}


struct Similar
{
    Similar() = default;
    virtual ~Similar() = default;

    NONCOPYABLE(Similar);

    virtual void Add(const std::string& line) = 0;
    virtual bool IsSame(const std::string& generated) = 0;
};


struct SimilarSet : public Similar
{
    std::set<std::string> existing_lines;

    void Add(const std::string& line) override
    {
        existing_lines.emplace(line);
    }

    bool IsSame(const std::string& generated) override
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
    std::set<std::string> rejected;

    void Add(const std::string& line) override
    {
        existing_lines.emplace_back(line);
        rejected.emplace(line);
    }

    bool IsSame(const std::string& generated) override
    {
        if(rejected.find(generated) != rejected.end())
        {
            return true;
        }

        const auto count = generated.size();
        return std::any_of
        (
            existing_lines.begin(), existing_lines.end(),
            [&](const auto& line)
            {
                const auto shortened = core::FirstChars(line, count);
                const int edits = core::fast_edit_distance(shortened, generated);
                if(edits < 10)
                {
                    rejected.emplace(generated);
                    return true;
                }
                return false;
            }
        );
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
    markov::chain_builder<std::string> m {memory};

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
            m.add(*p);
        }
    }

    std::cout << "\n";
    auto b = m.build();

    int skipped = 0;

    for(int i = 0; i < count+skipped; i += 1)
    {
        const auto generated = core::LineToString(b.generate(&rnd));
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

            if(!also_existing)
            {
                existing_lines->Add(generated);
            }
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
    core::argparse::parser parser {"markov tool"};

    auto sub = parser.add_sub_parsers();

    sub->add
    (
        "sentance",
        "parses and generates sentances",
        [&](core::argparse::sub_parser* sent)
        {
            std::string file;
            int memory = 4;
            int count = 25;

            sent->add("file", &file);
            sent->add("--memory", &memory);
            sent->add("--count", &count);

            return sent->on_complete([&]
            {
                MarkovSentance(file, memory, count);
                return core::argparse::ok;
            });
        }
    );

    sub->add
    (
        "word",
        "parses and generates word",
        [&](core::argparse::sub_parser* word)
        {
            std::string file;
            int memory = 4;
            int count = 25;

            word->add("file", &file);
            word->add("--memory", &memory);
            word->add("--count", &count);

            return word->on_complete([&]
            {
                MarkovWord(file, memory, count);
                return core::argparse::ok;
            });
        }
    );

    sub->add
    (
        "line",
        "parses and generates lines",
        [&](core::argparse::sub_parser* line_parser)
        {
            std::string file;
            int memory = 2;
            int count = 25;
            bool existing = false;
            bool simple = false;

            line_parser->add("file", &file);
            line_parser->add("--memory", &memory);
            line_parser->add("--count", &count);
            line_parser->add("--existing", &existing);
            line_parser->add("--simple", &simple);

            return line_parser->on_complete([&]
            {
                MarkovLine(file, memory, count, existing, simple);
                return core::argparse::ok;
            });
        }
    );


    return core::argparse::parse_from_main(&parser, argc, argv);
}
