#ifndef EUPHORIA_TEXTFILEPARSER_H
#define EUPHORIA_TEXTFILEPARSER_H

#include <string>

namespace euphoria::core
{
    bool
    IsIdentStart(const char c);

    struct TextFileParser
    {
    public:
        TextFileParser(const std::string& str);

        char
        PeekChar(unsigned int advance = 0);

        // like PeekChar but returns human readable strings for some chars
        std::string
        PeekString(unsigned int advance = 0);

        // if peekchar(0) is c then it is read and function returns true,
        // otherwise false
        bool
        ExpectChar(char c);

        void
        AdvanceChar();
        char
        ReadChar();
        std::string
        ReadIdent();
        std::string
        ReadString();
        std::string
        ReadToEndOfLine();

        void
        SkipSpaces(bool include_newline);
        bool
        HasMore() const;

        unsigned int
        GetLine();
        unsigned int
        GetColumn();

    private:
        std::string  string_;
        unsigned int length_;
        unsigned int position_;

        unsigned int line_;
        unsigned int column_;
    };

}  // namespace euphoria::core

#endif  // EUPHORIA_TEXTFILEPARSER_H
