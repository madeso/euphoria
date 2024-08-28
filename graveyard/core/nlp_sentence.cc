#include "core/nlp_sentence.h"


#include <iostream>
#include <string_view>

#include "base/stringutils.h"
#include "base/stringbuilder.h"


namespace eu::core
{


namespace
{

bool
is_char_lower(char c)
{
    return 'a' <= c && c <= 'z';
}


bool
is_char_upper(char c)
{
    return 'A' <= c && c <= 'Z';
}


bool
is_word_char(char c)
{
    // ' is using in words like can't
    // - is used in words like right-handed
    return is_char_upper(c) || is_char_lower(c) || is_number(c) || c == '\'' || c == '-';
}


bool
is_whitespace(char c)
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
is_end_of_sentence(char c)
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
is_comma_like(char c)
{
    constexpr std::string_view special_words = ",;:\"[]()=";
    return special_words.find(c) != std::string::npos;
}


int
get_char_code(char c)
{
    return static_cast<int>(static_cast<unsigned char>(c));
}


struct SentenceParser
{
    bool ok = true;
    std::string buffer;
    TextSentence words;
    core::OnSentenceFunction on_sentence;

    int line = 1;
    int ch = 0;

    void
    add_word()
    {
        if(!buffer.empty())
        {
            words.push_back(buffer);
            buffer = "";
        }
    }

    void
    on_unknown_character(char c)
    {
        std::cout << "Unknown character(" << line << ":" << ch << "): " << c
                  << " (" << get_char_code(c) << ")\n";
        ok = false;
    }

    void
    feed(char c)
    {
        if(get_char_code(c) >= 187)
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

        if(is_word_char(c))
        {
            buffer += c;
            return;
        }

        if(is_whitespace(c))
        {
            add_word();
            return;
        }

        if(is_comma_like(c))
        {
            add_word();
            words.push_back(std::string(1, c));
            return;
        }

        if(is_end_of_sentence(c))
        {
            add_word();
            words.push_back(std::string(1, c));
            on_sentence(words);
            words = TextSentence{};
            return;
        }

        on_unknown_character(c);
    }

    void
    on_complete()
    {
        if(words.empty() == false)
        {
            add_word();
            on_sentence(words);
            words = TextSentence{};
        }
    }
};
}

bool
parse_sentences(std::istream& data, OnSentenceFunction on_sentence)
{
    std::string line;

    SentenceParser parser;
    parser.on_sentence = on_sentence;

    while(std::getline(data, line))
    {
        if(line.empty())
        {
            continue;
        }

        for(char c: line)
        {
            parser.feed(c);
        }
        parser.feed('\n');

        if(!parser.ok)
        {
            return false;
        }
    }

    parser.on_complete();

    return parser.ok;
}


std::string
from_sentence_to_string(const TextSentence& s)
{
    auto ss = StringBuilder{};
    bool first = true;

    for(const auto& w: s)
    {
        if(first)
        {
            first = false;
        }
        else
        {
            if(is_comma_like(w[0]) || is_end_of_sentence(w[0]))
            {
            }
            else
            {
                ss.add_char(' ');
            }
        }

        ss.add_string(w);
    }

    return ss.to_string();
}

}
