#include "core/textfileparser.h"

#include "core/numeric.h"
#include "core/assert.h"

#include <sstream>


namespace euphoria::core
{
    namespace detail
    {
        char
        textfile::peek()
        {
            return peek(1);
        }

        struct textfile_string : public textfile
        {
            std::string text;
            std::size_t next_position = 0;

            [[nodiscard]]
            bool
            has_more() const override
            {
                return next_position < text.size();
            }

            char
            peek(int advance) override
            {
                ASSERT(advance >= 0);
                const auto index = next_position + advance;

                if (index >= text.size())
                {
                    return 0;
                }
                else
                {
                    return text[index];
                }
            }

            char
            read() override
            {
                if (!has_more())
                {
                    return 0;
                }

                const auto r = text[next_position];
                next_position += 1;
                return r;
            }
        };

        std::shared_ptr<textfile>
        create_from_string(const std::string& str)
        {
            auto file = std::make_shared<textfile_string>();
            file->text = str;
            return file;
        }
    }

    namespace  // local
    {
        bool
        is_newline(const char c)
        {
            if(c == '\n')
            {
                return true;
            }

            if(c == '\r')
            {
                return true;
            }

            return false;
        }

        std::string
        char_to_string(char c)
        {
            std::ostringstream ss;
            switch (c)
            {
            case 0: ss << "<null>"; break;
            case ' ': ss << "<space>"; break;
            case '\n': ss << "<newline>"; break;
            case '\t': ss << "<tab>"; break;
            default: ss << c; break;
            }
            return ss.str();
        }
    }

    textfile_parser::textfile_parser(std::shared_ptr<detail::textfile> afile)
        : file(afile)
    {
    }

    textfile_parser
    textfile_parser::from_string(const std::string& str)
    {
        return textfile_parser{ detail::create_from_string(str) };
    }

    char
    textfile_parser::peek_char(int advance)
    {
        return file->peek(advance);
    }


    std::string
    textfile_parser::peek_string(int advance)
    {
        const auto c = peek_char(advance);
        return char_to_string(c);
    }

    // if peekchar(0) is c then it is read and function returns true,
    // otherwise false
    bool
    textfile_parser::expect_char(char c)
    {
        if(peek_char() == c)
        {
            read_char();
            return true;
        }
        else
        {
            return false;
        }
    }


    char
    textfile_parser::read_char()
    {
        const char r = file->read();

        if(is_newline(r))
        {
            location.column = 1;
            location.line += 1;
        }
        else
        {
            location.column += 1;
        }

        return r;
    }

    void
    textfile_parser::advance_char()
    {
        read_char();
    }

    namespace  // local
    {
        bool
        is_ident_char(char c, bool first_char)
        {
            if(is_within_inclusive_as_int('a', c, 'z'))
            {
                return true;
            }
            if(is_within_inclusive_as_int('A', c, 'Z'))
            {
                return true;
            }
            if(is_within_inclusive_as_int('0', c, '9'))
            {
                return !first_char;
            }
            if(c == '_')
            {
                return true;
            }

            return false;
        }
    }  // namespace

    bool
    is_ident_start(char c)
    {
        return is_ident_char(c, true);
    }

    std::string
    textfile_parser::read_ident()
    {
        std::ostringstream ss;
        bool               first = true;
        while(is_ident_char(peek_char(), first))
        {
            first = false;
            ss << read_char();
        }
        return ss.str();
    }

    std::string
    textfile_parser::read_string()
    {
        std::ostringstream ss;
        const char         quote = '\"';
        if(peek_char() != quote)
        {
            return "";
        }
        advance_char();  // skip " char
        while(peek_char() != quote)
        {
            const char c = read_char();
            if(c == '\\')
            {
                const char nc = read_char();
                if(nc == '\\')
                {
                    ss << '\\';
                }
                else if(nc == 't')
                {
                    ss << '\t';
                }
                else if(nc == '\n')
                {
                    ss << '\n';
                }
                else if(nc == '\"')
                {
                    ss << '\"';
                }
                else
                {
                    ss << nc;
                }
            }
            else
            {
                ss << c;
            }
        }
        const char c = read_char();
        if(c != quote)
        {
            return "";
        }
        return ss.str();
    }

    std::string
    textfile_parser::read_to_end_of_line()
    {
        std::ostringstream ss;
        while(!is_newline(peek_char()))
        {
            const char c = read_char();
            if(c == 0)
            {
                return ss.str();
            }
            ss << c;
        }
        advance_char();  // skip the newline
        return ss.str();
    }

    namespace  // local
    {
        bool
        is_space_character(char c, bool include_newline)
        {
            switch(c)
            {
            case ' ':
            case '\t': return true;
            case '\n':
            case '\r': return include_newline;
            default: return false;
            }
        }
    }  // namespace

    void
    textfile_parser::skip_spaces(bool include_newline)
    {
        while(is_space_character(peek_char(), include_newline))
        {
            read_char();
        }
    }

    bool
    textfile_parser::has_more() const
    {
        return file->has_more();
    }

    int
    textfile_parser::get_line() const
    {
        return location.line;
    }

    int
    textfile_parser::get_column() const
    {
        return location.column;
    }

}

