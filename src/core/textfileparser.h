#ifndef EUPHORIA_TEXTFILEPARSER_H
#define EUPHORIA_TEXTFILEPARSER_H

#include <string>
#include <memory>

namespace euphoria::core
{
    namespace detail
    {
        struct TextFile
        {
            virtual ~TextFile() = default;

            virtual bool
            HasMore() const = 0;

            // return 0 if position is beyond file
            virtual char
            Peek(int advance = 1) = 0;

            virtual char
            Read() = 0;
        };

        std::shared_ptr<TextFile>
        FromString(const std::string& str);

        struct Location
        {
            int line = -1;
            int column = -1;
        };
    }

    bool
    IsIdentStart(const char c);

    /** Parses a text file in memory.
     */
    struct TextFileParser
    {
        TextFileParser(std::shared_ptr<detail::TextFile> afile);

        static TextFileParser
        FromString(const std::string& str);

        
        // advance = 0 - next char, 1-the one after that, negative values are not allowed
        char
        PeekChar(int advance = 0);

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

        int
        GetLine();

        int
        GetColumn();

        std::shared_ptr<detail::TextFile> file;
        detail::Location location = detail::Location{ 1, 1 };
    };

}  // namespace euphoria::core

#endif  // EUPHORIA_TEXTFILEPARSER_H
