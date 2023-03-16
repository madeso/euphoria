
#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>

#include "core/markov.h"
#include "core/argparse.h"
#include "core/stringmerger.h"
#include "core/stringutils.h"
#include "core/editdistance.h"
#include "core/nlp_sentence.h"
#include "core/nlp_line.h"

namespace core = euphoria::core;
namespace markov = euphoria::core::markov;


std::vector<char>
c(const std::string& str)
{
    return std::vector<char> {str.begin(), str.end()};
}


std::string
c(const std::vector<char>& v)
{
    return std::string {v.begin(), v.end()};
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void
markov_sentence(const std::string& file, int memory, int count)
{
    std::ifstream data;
    data.open(file);
    if(!data)
    {
        std::cerr << "Failed to open input\n";
        return;
    }

    markov::ChainBuilder<std::string> m {memory};
    const auto parsed = core::parse_sentences
    (
        data,
        [&](const core::TextSentence& s) { m.add(s); }
    );
    if(!parsed)
    {
        std::cerr << "No sentences loaded\n";
        return;
    }

    core::Random rnd;
    auto b = m.build();

    for(int i = 0; i < count; i += 1)
    {
        auto s = b.generate(&rnd);
        std::cout << core::sentence_to_string(s) << "\n\n";
    }
}


void
markov_word(const std::string& file, int memory, int count)
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
        if(line.empty()) { continue; }
        m.add(c(line));
    }

    std::cout << "\n";
    auto b = m.build();

    for(int i = 0; i < count; i += 1)
    {
        std::cout << c(b.generate(&rnd)) << "\n";
    }
}


struct Similar
{
    Similar() = default;
    virtual ~Similar() = default;

    Similar(const Similar& other) = delete;
    void operator=(const Similar&) = delete;
    Similar(Similar&& other) = delete;
    void operator=(Similar&&) = delete;

    virtual void add(const std::string& line) = 0;
    virtual bool is_same(const std::string& generated) = 0;
};


struct SimilarSet : public Similar
{
    std::set<std::string> existing_lines;

    void add(const std::string& line) override
    {
        existing_lines.emplace(line);
    }

    bool is_same(const std::string& generated) override
    {
        return existing_lines.find(generated) != existing_lines.end();
    }

    static std::unique_ptr<Similar> create()
    {
         return std::make_unique<SimilarSet>();
    }
};


struct SimilarEditDistance : public Similar
{
    std::vector<std::string> existing_lines;
    std::set<std::string> rejected;

    void add(const std::string& line) override
    {
        existing_lines.emplace_back(line);
        rejected.emplace(line);
    }

    bool is_same(const std::string& generated) override
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
                const auto shortened = core::first_chars(line, count);
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

    static std::unique_ptr<Similar> create()
    {
         return std::make_unique<SimilarEditDistance>();
    }
};


void
markov_line(const std::string& file, int memory, int count, bool also_existing, bool simple)
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

    std::unique_ptr<Similar> existing_lines = simple ? SimilarSet::create() : SimilarEditDistance::create();

    {
        std::string line;
        while(std::getline(data, line))
        {
            if(line.empty()) { continue; }
            const auto p = core::parse_line(line);
            if(!p) { continue; }

            if(!also_existing)
            {
                existing_lines->add(core::to_lower(line));
            }
            m.add(*p);
        }
    }

    std::cout << "\n";
    auto b = m.build();

    int skipped = 0;

    for(int i = 0; i < count+skipped; i += 1)
    {
        const auto generated = core::line_to_string(b.generate(&rnd));
        if(!also_existing && existing_lines->is_same(core::to_lower(generated)))
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
                existing_lines->add(generated);
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
    core::argparse::Parser parser {"markov tool"};

    auto sub = parser.add_sub_parsers();

    sub->add
    (
        "sentence",
        "parses and generates sentences",
        [&](core::argparse::SubParser* sent)
        {
            std::string file;
            int memory = 4;
            int count = 25;

            sent->add("file", &file);
            sent->add("--memory", &memory);
            sent->add("--count", &count);

            return sent->on_complete([&]
            {
                markov_sentence(file, memory, count);
                return core::argparse::ok;
            });
        }
    );

    sub->add
    (
        "word",
        "parses and generates word",
        [&](core::argparse::SubParser* word)
        {
            std::string file;
            int memory = 4;
            int count = 25;

            word->add("file", &file);
            word->add("--memory", &memory);
            word->add("--count", &count);

            return word->on_complete([&]
            {
                markov_word(file, memory, count);
                return core::argparse::ok;
            });
        }
    );

    sub->add
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

            line_parser->add("file", &file);
            line_parser->add("--memory", &memory);
            line_parser->add("--count", &count);
            line_parser->add("--existing", &existing);
            line_parser->add("--simple", &simple);

            return line_parser->on_complete([&]
            {
                markov_line(file, memory, count, existing, simple);
                return core::argparse::ok;
            });
        }
    );


    return core::argparse::parse_from_main(&parser, argc, argv);
}
