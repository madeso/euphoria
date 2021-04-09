#include "core/nlp_sentance.h"

#include <sstream>
#include <iostream>

#include "core/stringutils.h"


namespace euphoria::core
{


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
IsWordChar(char c)
{
    // ' is using in words like can't
    // - is used in words like right-handed
    return IsCharUpper(c) || IsCharLower(c) || IsNumber(c) || c == '\'' || c == '-';
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


int
CharCode(char c)
{
    return static_cast<int>(static_cast<unsigned char>(c));
}


struct Parser
{
    bool ok = true;
    std::string buffer;
    Sentance words;
    OnSentance on_sentance;

    int line = 1;
    int ch = 0;

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
        {
            return;
        }

        if(c == '\n')
        {
            line += 1;
            ch = 0;
        }
        else
        {
            ch += 1;
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
    }

    void
    OnComplete()
    {
        if(words.empty() == false)
        {
            AddWord();
            on_sentance(words);
            words = Sentance {};
        }
    }
};


bool
ParseSentances(std::istream& data, OnSentance on_sentance)
{
    std::string line;

    Parser parser;
    parser.on_sentance = on_sentance;

    while(std::getline(data, line))
    {
        if(line.empty())
        {
            continue;
        }

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

    parser.OnComplete();

    return parser.ok;
}


std::string
SentanceToString(const Sentance& s)
{
    std::ostringstream ss;
    bool first = true;

    for(const auto& w: s)
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

}
