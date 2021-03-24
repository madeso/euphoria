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
            TextFile() = default;
            virtual ~TextFile() = default;

            TextFile(const TextFile&) = delete;
            TextFile(TextFile&&) = delete;
            void operator=(const TextFile&) = delete;
            void operator=(TextFile&&) = delete;

            [[nodiscard]]
            virtual bool
            HasMore() const = 0;

            // return 0 if position is beyond file
            [[nodiscard]]
            virtual char
            Peek(int advance) = 0;

            char
            Peek(/* advance = 1 */);

            [[nodiscard]]
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
    IsIdentStart(char c);

    /** Parses a text file in memory.
     */
    struct TextFileParser
    {
        explicit TextFileParser(std::shared_ptr<detail::TextFile> afile);

        static TextFileParser
        FromString(const std::string& str);

        
        // advance = 0 - next char, 1-the one after that, negative values are not allowed
        char
        PeekChar(int advance = 0);

        // like PeekChar but returns human readable strings for some chars
        std::string
        PeekString(int advance = 0);

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

        [[nodiscard]]
        bool
        HasMore() const;

        [[nodiscard]]
        int
        GetLine() const;

        [[nodiscard]]
        int
        GetColumn() const;

        std::shared_ptr<detail::TextFile> file;
        detail::Location location = detail::Location{ 1, 1 };
    };

}  // namespace euphoria::core

#endif  // EUPHORIA_TEXTFILEPARSER_H
