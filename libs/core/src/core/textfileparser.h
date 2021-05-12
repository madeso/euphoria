#pragma once

#include <string>
#include <memory>

namespace euphoria::core
{
    namespace detail
    {
        struct textfile
        {
            textfile() = default;
            virtual ~textfile() = default;

            textfile(const textfile&) = delete;
            textfile(textfile&&) = delete;
            void operator=(const textfile&) = delete;
            void operator=(textfile&&) = delete;

            [[nodiscard]]
            virtual bool
            has_more() const = 0;

            // return 0 if position is beyond file
            [[nodiscard]]
            virtual char
            peek(int advance) = 0;

            char
            peek(/* advance = 1 */);

            [[nodiscard]]
            virtual char
            read() = 0;
        };

        std::shared_ptr<textfile>
        create_from_string(const std::string& str);

        struct location
        {
            int line = -1;
            int column = -1;
        };
    }

    bool
    is_ident_start(char c);

    /** Parses a text file in memory.
     */
    struct textfile_parser
    {
        explicit textfile_parser(std::shared_ptr<detail::textfile> afile);

        static textfile_parser
        from_string(const std::string& str);

        
        // advance = 0 - next char, 1-the one after that, negative values are not allowed
        char
        peek_char(int advance = 0);

        // like PeekChar but returns human readable strings for some chars
        std::string
        peek_string(int advance = 0);

        // if peekchar(0) is c then it is read and function returns true,
        // otherwise false
        bool
        expect_char(char c);

        void
        advance_char();

        char
        read_char();

        std::string
        read_ident();

        std::string
        read_string();

        std::string
        read_to_end_of_line();

        void
        skip_spaces(bool include_newline);

        [[nodiscard]]
        bool
        has_more() const;

        [[nodiscard]]
        int
        get_line() const;

        [[nodiscard]]
        int
        get_column() const;

        std::shared_ptr<detail::textfile> file;
        detail::location location = detail::location{ 1, 1 };
    };

}

