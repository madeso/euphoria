#pragma once


#include <memory>

namespace euphoria::core
{
    namespace detail
    {
        struct Textfile
        {
            Textfile() = default;
            virtual ~Textfile() = default;

            Textfile(const Textfile&) = delete;
            Textfile(Textfile&&) = delete;
            void operator=(const Textfile&) = delete;
            void operator=(Textfile&&) = delete;

            char peek(/* advance = 1 */);

            [[nodiscard]] virtual bool has_more() const = 0;
            // return 0 if position is beyond file
            [[nodiscard]] virtual char peek(int advance) = 0;
            [[nodiscard]] virtual char read() = 0;
        };

        std::shared_ptr<Textfile> create_from_string(const std::string& str);

        struct LocationInFile
        {
            int line = -1;
            int column = -1;
        };
    }

    bool
    is_ident_start(char c);

    /** Parses a text file in memory.
     */
    struct TextfileParser
    {
        std::shared_ptr<detail::Textfile> file;
        detail::LocationInFile location = detail::LocationInFile{ 1, 1 };


        explicit TextfileParser(std::shared_ptr<detail::Textfile> afile);


        static TextfileParser from_string(const std::string& str);

        
        /// advance = 0 - next char, 1-the one after that, negative values are not allowed
        char peek_char(int advance = 0);

        /// like PeekChar but returns human readable strings for some chars
        std::string peek_string(int advance = 0);

        /// if peekchar(0) is c then it is read and function returns true, otherwise false
        bool expect_char(char c);

        void advance_char();
        char read_char();
        std::string read_ident();
        std::string read_string();
        std::string read_to_end_of_line();
        void skip_spaces(bool include_newline);


        [[nodiscard]] bool has_more() const;
        [[nodiscard]] int get_line() const;
        [[nodiscard]] int get_column() const;
    };

}

