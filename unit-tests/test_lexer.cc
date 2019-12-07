#include "core/lexer.h"
#include "catch.hpp"
#include <vector>
#include <string>
#include <array>

#include "core/assert.h"

using namespace euphoria::core::lexer;

namespace
{
    enum MyTokens
    {
        CT_FIRST = TOKEN_LAST,

        CT_IF, CT_WHILE, CT_PP, CT_P
    };
}

namespace euphoria::core::lexer
{
    bool operator==(const Location& lhs, const Location& rhs)
    {
        return lhs.line   == rhs.line
            && lhs.column == rhs.column;
    }
    bool operator!=(const Location& lhs, const Location& rhs)
    { return !(lhs == rhs); }

    std::ostream& operator<<(std::ostream& s, const Location& loc)
    {
        s << "(" << loc.column << " " << loc.line << ")";
        return s;
    }
}

namespace
{
    template<typename T>
    auto Eq( std::vector<T> const& comparator )
    {
        return Catch::Matchers::Equals(comparator);
    }

    // mirrors Token
    struct T
    {
        explicit T(int a_type) : type(a_type) {}
        T(int a_type, const std::string& a_text)
            : type(a_type), text(a_text) {}
        explicit T(const Token& t)
            : type(t.type)
            , text(t.text)
            , start(t.start)
            , end(t.end)
        {}

        int type;
        std::optional<std::string> text;

        std::optional<Location> start;
        std::optional<Location> end;
    };

    // mirrors Error
    struct E
    {
        explicit E(const Error& e)
            : location(e.location)
            , message(e.message)
        { }
        std::optional<Location> location;
        std::optional<std::string> message;
    };

    bool operator==(const T& lhs, const T& rhs);
    bool operator!=(const T& lhs, const T& rhs)
    { return !(lhs == rhs); }
    bool operator==(const T& lhs, const T& rhs)
    {
        if(lhs.type != rhs.type) return false;
#define PROP_DIFF(T) (lhs.T && rhs.T && *lhs.T != *rhs.T)
        if(PROP_DIFF(text)) return false;
        if(PROP_DIFF(start)) return false;
        if(PROP_DIFF(end)) return false;
#undef PROP_DIFF
        return true;
    }

    bool operator==(const E& lhs, const E& rhs);
    bool operator!=(const E& lhs, const E& rhs)
    { return !(lhs == rhs); }
    bool operator==(const E& lhs, const E& rhs)
    {
#define PROP_DIFF(T) (lhs.T && rhs.T && *lhs.T != *rhs.T)
        if(PROP_DIFF(location)) return false;
        if(PROP_DIFF(message)) return false;
#undef PROP_DIFF
        return true;
    }

    std::ostream& operator<<(std::ostream& s, const T& tok)
    {
        enum class Display { No, Yes, Quote };
        const auto [type_string, display_text] = [&]() {
            switch(tok.type)
            {
            case TOKEN_WHITESPACE: return std::make_pair("whitespace", Display::Yes);
            case TOKEN_IDENTIFIER: return std::make_pair("",           Display::Yes);
            case TOKEN_INT       : return std::make_pair("int",        Display::Yes);
            case TOKEN_NUMBER    : return std::make_pair("number",     Display::Yes);
            case TOKEN_STRING    : return std::make_pair("string",     Display::Quote);
            case CT_IF           : return std::make_pair("if",         Display::No);
            case CT_WHILE        : return std::make_pair("while",      Display::No);
            case CT_PP           : return std::make_pair("++",         Display::No);
            case CT_P            : return std::make_pair("+",          Display::No);
            default              : DIE("unhandled type");
                                   return std::make_pair("<other>",    Display::Quote);
            }
        } ();
        s << type_string;

        if(tok.text)
        {
            switch(display_text)
            {
            case Display::No:
                break;
            case Display::Yes:
                s << " " << *tok.text;
                break;
            case Display::Quote:
                s << " " << ::Catch::Detail::stringify(*tok.text);
                break;
            default:
                assert(false && "unhandled case");
            }
        }

        return s;
    }

    std::ostream& operator<<(std::ostream& s, const E& err)
    {
        if(err.location)
        {
            s << *err.location;
        }
        else
        {
            s << "<no location>";
        }
        s << ": ";
        s << err.message.value_or("<no message>");
        return s;
    }
}

namespace
{
    using Ts = std::vector<T>;
    using Es = std::vector<E>;

    std::pair<Ts, Es>
    Parse(const Lexer& lexer, const std::string& source)
    {
        auto tokens = Ts{};
        auto errors = Es{};

        lexer.Parse(
            FromString(source),
            [&](const Token& t) { tokens.emplace_back(t); },
            [&](const Error& e) { errors.emplace_back(e); }
        );

        return {tokens, errors};
    }
}

TEST_CASE("lexer_basic")
{
    const auto basic = Lexer{};

    const auto with_keywords
        = Lexer{}
        .AddToken("if", CT_IF)
        .AddToken("while", CT_WHILE)
        .AddToken("+", CT_P)
        .AddToken("++", CT_PP)
        ;
    const auto with_keywords_reverse
        = Lexer{}
        .AddToken("++", CT_PP)
        .AddToken("+", CT_P)
        .AddToken("while", CT_WHILE)
        .AddToken("if", CT_IF)
        ;

    // replace with a single GENERATE call?
    const auto lexers = std::array{basic, with_keywords, with_keywords_reverse};
    const auto lexer_index = GENERATE(0, 1, 2);
    auto lexer = lexers[lexer_index];

    SECTION("empty without spaces")
    {
        const auto [tokens, errors] = Parse(lexer, "");
        CHECK_THAT(tokens, Eq(Ts{}));
        CHECK_THAT(errors, Eq(Es{}));
    }

    SECTION("empty with spaces")
    {
        const auto [tokens, errors] = Parse(lexer, " \t \n \r");
        CHECK_THAT(tokens, Eq(Ts{}));
        CHECK_THAT(errors, Eq(Es{}));
    }

    SECTION("single line comment")
    {
        const auto [tokens, errors] = Parse(lexer, "// hello world");
        CHECK_THAT(tokens, Eq(Ts{}));
        CHECK_THAT(errors, Eq(Es{}));
    }

    SECTION("multi line comment")
    {
        const auto [tokens, errors] = Parse(lexer, "/* hello world */");
        CHECK_THAT(tokens, Eq(Ts{}));
        CHECK_THAT(errors, Eq(Es{}));
    }
}
