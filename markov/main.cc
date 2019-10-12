#include <string>
#include <iostream>
#include <fstream>

#include "core/dots.h"
#include "core/markov.h"
#include "core/argparse.h"

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

struct Sentance
{};

bool
IsWordCharLower(char c)
{
    return 'a' <= c && c <= 'z';
}

bool
IsWordCharUpper(char c)
{
    return 'A' <= c && c <= 'Z';
}

bool
IsWordCharNumber(char c)
{
    return '0' <= c && c <= '9';
}

bool
IsWordChar(char c)
{
    // ' is using in words like can't
    // - is used in words like right-handed
    return IsWordCharUpper(c) || IsWordCharLower(c) || IsWordCharNumber(c)
           || c == '\'' || c == '-';
}

struct Parser
{
    bool        word = false;
    std::string buffer;

    std::vector<std::string> words;

    void
    Feed(char c)
    {
        if(IsWordChar(c))
        {
            buffer += c;
            word = true;
            return;
        }
        if(c == ' ')
        {
            WordIsDone();
            return;
        }
        if(c == ',')
        {
            WordIsDone();
            return;
        }
        if(c == ';')
        {
            WordIsDone();
            return;
        }
        if(c == ':')
        {
            WordIsDone();
            return;
        }
        if(c == '=')
        {
            WordIsDone();
            return;
        }
        if(c == '.')
        {
            WordIsDone();
            return;
        }
        if(c == '!')
        {
            WordIsDone();
            return;
        }
        if(c == '?')
        {
            WordIsDone();
            return;
        }
        if(c == '"')
        {
            WordIsDone();
            return;
        }
        if(c == '[')
        {
            WordIsDone();
            return;
        }
        if(c == ']')
        {
            WordIsDone();
            return;
        }
        if(c == '(')
        {
            WordIsDone();
            return;
        }
        if(c == ')')
        {
            WordIsDone();
            return;
        }
        if(c == '\n')
        {
            WordIsDone();
            return;
        }

        throw "unhandled char";
    }

    void
    WordIsDone()
    {
        if(word)
        {
            if(!buffer.empty())
            {
                words.emplace_back(buffer);
            }
            buffer = "";
        }
    }
};

std::vector<Sentance>
ParseSentances(std::ifstream& data)
{
    std::vector<Sentance> r;

    std::string line;
    core::Dots        dots;

    Parser parser;

    while(std::getline(data, line))
    {
        if(line.empty())
            continue;
        dots.Dot(100);

        for(char c: line)
        {
            parser.Feed(c);
        }
        parser.Feed('\n');
    }

    return r;
}

void
MarkovSentance(const std::string& file, int memory)
{
    core::Random                    rnd;
    markov::ChainBuilder<Sentance> m {memory};

    std::ifstream data;
    data.open(file);
    if(!data)
    {
        std::cerr << "Failed to open input\n";
        return;
    }

    const auto sents = ParseSentances(data);

}

void
MarkovWord(const std::string& file, int memory, int count)
{
    core::Random                rnd;
    markov::ChainBuilder<char> m {memory};

    std::ifstream data;
    data.open(file);
    if(!data)
    {
        std::cerr << "Failed to open input\n";
        return;
    }

    std::string line;
    core::Dots        dots;
    while(std::getline(data, line))
    {
        if(line.empty())
            continue;
        m.Add(C(line));
        dots.Dot(100);
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

    std::string file;
    int memory = 4;
    int count = 25;

    auto sent = parser.AddSubParser("sentance", "parses and generates sentances", [&](){
        MarkovSentance(file, memory);
    });
    sent->AddSimple("file", &file);
    sent->AddSimple("--memory", &memory);

    auto word = parser.AddSubParser("word", "parses and generates word", [&](){
        MarkovWord(file, memory, count);
    });
    word->AddSimple("file", &file);
    word->AddSimple("--memory", &memory);
    word->AddSimple("--count", &count);

    auto result = parser.Parse(argc, argv);
    if(result == core::argparse::ParseResult::Failed)
    {
        return -2;
    }
    else
    {
        return 0;
    }
    
}
