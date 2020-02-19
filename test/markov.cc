#include <string>
#include <iostream>
#include <fstream>

#include "core/cli_progress_dots.h"
#include "core/markov.h"
#include "core/argparse.h"
#include "core/stringmerger.h"

namespace core   = euphoria::core;
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

using Sentance = std::vector<std::string>;

bool
IsCharLower(char c)
{
    return 'a' <= c && c <= 'z';
}

bool
IsCharUpper(char c)
{
    return 'A' <= c && c <= 'Z';
}

bool
IsNumber(char c)
{
    return '0' <= c && c <= '9';
}

bool
IsWordChar(char c)
{
    // ' is using in words like can't
    // - is used in words like right-handed
    return IsCharUpper(c) || IsCharLower(c) || IsNumber(c) || c == '\''
           || c == '-';
}

bool
IsWhitespace(char c)
{
    switch(c)
    {
    case ' ':
    case '\n':
    case '\t':
    case '\r': return true;
    default: return false;
    }
}

bool
IsEndOfSentance(char c)
{
    switch(c)
    {
    case '.':
    case '!':
    case '?': return true;

    default: return false;
    }
}

bool
IsCommaLike(char c)
{
    const std::string SPECIAL_WORDS = ",;:\"[]()=";
    return SPECIAL_WORDS.find(c) != std::string::npos;
}

using OnSentance = std::function<void(const Sentance&)>;

int
CharCode(char c)
{
    return static_cast<int>(static_cast<unsigned char>(c));
}

struct Parser
{
    bool        ok = true;
    std::string buffer;
    Sentance    words;
    OnSentance  on_sentance;

    int line = 1;
    int ch   = 0;

    void
    AddWord()
    {
        if(!buffer.empty())
        {
            words.push_back(buffer);
            buffer = "";
        }
    }

    void
    UnknownCharacter(char c)
    {
        std::cout << "Unknown character(" << line << ":" << ch << "): " << c
                  << " (" << CharCode(c) << ")\n";
        ok = false;
    }

    void
    Feed(char c)
    {
        if(CharCode(c) >= 187)
            return;

        if(c == '\n')
        {
            line += 1;
            ch = 0;
        }
        else
        {
            ch += 1;
        }

        if(words.empty())
        {
            if(IsWhitespace(c))
            {
                return;
            }
        }

        if(IsWordChar(c))
        {
            buffer += c;
            return;
        }

        if(IsWhitespace(c))
        {
            AddWord();
            return;
        }

        if(IsCommaLike(c))
        {
            AddWord();
            words.push_back(std::string(1, c));
            return;
        }

        if(IsEndOfSentance(c))
        {
            AddWord();
            words.push_back(std::string(1, c));
            on_sentance(words);
            words = Sentance {};
            return;
        }

        UnknownCharacter(c);
        return;
    }
};

bool
ParseSentances(std::ifstream& data, OnSentance on_sentance)
{
    std::string line;
    core::CliProgressDots dots;

    Parser parser;
    parser.on_sentance = on_sentance;

    while(std::getline(data, line))
    {
        if(line.empty())
            continue;
        dots.Update();

        for(char c: line)
        {
            parser.Feed(c);
        }
        parser.Feed('\n');

        if(!parser.ok)
        {
            return false;
        }
    }
    std::cout << "\n";

    return parser.ok;
}

std::string
SentanceToString(const Sentance& s)
{
    std::ostringstream ss;
    bool               first = true;

    for(const auto w: s)
    {
        if(first)
        {
            first = false;
        }
        else
        {
            if(IsCommaLike(w[0]) || IsEndOfSentance(w[0]))
            {
            }
            else
            {
                ss << " ";
            }
        }

        ss << w;
    }

    return ss.str();
}

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
    const auto                        parsed
            = ParseSentances(data, [&](const Sentance& s) { m.Add(s); });
    if(!parsed)
    {
        std::cerr << "No sentances loaded\n";
        return;
    }

    core::Random rnd;
    auto         b = m.Build();

    for(int i = 0; i < count; i += 1)
    {
        auto s = b.Generate(&rnd);
        std::cout << SentanceToString(s) << "\n\n";
    }
}

void
MarkovWord(const std::string& file, int memory, int count)
{
    core::Random               rnd;
    markov::ChainBuilder<char> m {memory};

    std::ifstream data;
    data.open(file);
    if(!data)
    {
        std::cerr << "Failed to open input\n";
        return;
    }

    std::string line;
    core::CliProgressDots  dots;
    while(std::getline(data, line))
    {
        if(line.empty())
            continue;
        m.Add(C(line));
        dots.Update();
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
    int         memory = 4;
    int         count  = 25;

    auto sent = parser.AddSubParser(
            "sentance", "parses and generates sentances", [&]() {
                MarkovSentance(file, memory, count);
            });
    sent->AddSimple("file", &file);
    sent->AddSimple("--memory", &memory);
    sent->AddSimple("--count", &count);

    auto word = parser.AddSubParser("word", "parses and generates word", [&]() {
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
