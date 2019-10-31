#ifndef EUPHHORIA_CORE_LEXER_H
#define EUPHHORIA_CORE_LEXER_H

#include <memory>
#include <string>
#include <functional>
#include <map>
#include <optional>

// todo(Gustav): merge this with
// textfileparser
// tracery
// stringtable (csv)
// template
// and other custom Parse/Peek functions and parserthings
//

namespace euphoria::core::lexer
{

struct File
{
    virtual ~File() = default;

    virtual bool
    HasMore() const = 0;

    // return 0 if position is beyond file
    virtual char
    Peek(std::size_t advance = 1) = 0;

    virtual char
    Read() = 0;
};

std::shared_ptr<File>
FromString(const std::string& str);

struct Location
{
    int line = -1;
    int column = -1;
};

struct Token
{
    int type;
    std::string text;

    Location start;
    Location end;
};

struct Error
{
    Location location;
    std::string message;
};

using CustomToken = std::function<Token (std::shared_ptr<File>)>;

enum TokenType
{
    TOKEN_WHITESPACE,
    TOKEN_IDENTIFIER,
    TOKEN_INT,
    TOKEN_NUMBER,
    TOKEN_STRING,
    // begin your tokens at
    TOKEN_LAST
};

namespace detail
{
    struct Node;

    struct Children
    {
        std::shared_ptr<Node>
        Find(char c);

        std::shared_ptr<Node>
        GetOrCreate(char c);

        std::map<char, std::shared_ptr<Node>> nodes;
    };

    struct Node
    {
        bool
        IsTerminal() const;

        std::optional<int> token;
        std::shared_ptr<CustomToken> custom;

        std::string debug_name;
        Children children;
    };
}

struct Lexer
{
    Lexer&
    AddToken(const std::string& token, int token_type);
    
    Lexer&
    AddToken(const std::string& debug_name, const std::string& token, CustomToken whitespace);

    void
    Parse
        (
            std::shared_ptr<File> file,
            std::function<void(const Token&)> on_token,
            std::function<void(const Error&)> on_error
        ) const;

    detail::Children children;
};

}


#endif  // EUPHHORIA_CORE_LEXER_H
