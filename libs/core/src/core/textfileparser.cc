#include "core/textfileparser.h"

#include "core/numeric.h"
#include "core/assert.h"

#include <sstream>


namespace euphoria::core
{
    namespace detail
    {
        char
        TextFile::Peek()
        {
            return Peek(1);
        }

        struct FileString : public TextFile
        {
            std::string text;
            std::size_t next_position = 0;

            [[nodiscard]]
            bool
            HasMore() const override
            {
                return next_position < text.size();
            }

            char
            Peek(int advance) override
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
            Read() override
            {
                if (!HasMore())
                {
                    return 0;
                }

                const auto r = text[next_position];
                next_position += 1;
                return r;
            }
        };

        std::shared_ptr<TextFile>
        FromString(const std::string& str)
        {
            auto file = std::make_shared<FileString>();
            file->text = str;
            return file;
        }
    }

    namespace  // local
    {
        bool
        IsNewline(const char c)
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
            CharToString(char c)
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
    }  // namespace

    TextFileParser::TextFileParser(std::shared_ptr<detail::TextFile> afile)
        : file(afile)
    {
    }

    TextFileParser
    TextFileParser::FromString(const std::string& str)
    {
        return TextFileParser{ detail::FromString(str) };
    }

    char
    TextFileParser::PeekChar(int advance)
    {
        return file->Peek(advance);
    }


    std::string
    TextFileParser::PeekString(int advance)
    {
        const auto c = PeekChar(advance);
        return CharToString(c);
    }

    // if peekchar(0) is c then it is read and function returns true,
    // otherwise false
    bool
    TextFileParser::ExpectChar(char c)
    {
        if(PeekChar() == c)
        {
            ReadChar();
            return true;
        }
        else
        {
            return false;
        }
    }


    char
    TextFileParser::ReadChar()
    {
        const char r = file->Read();

        if(IsNewline(r))
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
    TextFileParser::AdvanceChar()
    {
        ReadChar();
    }

    namespace  // local
    {
        bool
        IsIdentChar(char c, bool first_char)
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
    IsIdentStart(char c)
    {
        return IsIdentChar(c, true);
    }

    std::string
    TextFileParser::ReadIdent()
    {
        std::ostringstream ss;
        bool               first = true;
        while(IsIdentChar(PeekChar(), first))
        {
            first = false;
            ss << ReadChar();
        }
        return ss.str();
    }

    std::string
    TextFileParser::ReadString()
    {
        std::ostringstream ss;
        const char         quote = '\"';
        if(PeekChar() != quote)
        {
            return "";
        }
        AdvanceChar();  // skip " char
        while(PeekChar() != quote)
        {
            const char c = ReadChar();
            if(c == '\\')
            {
                const char nc = ReadChar();
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
        const char c = ReadChar();
        if(c != quote)
        {
            return "";
        }
        return ss.str();
    }

    std::string
    TextFileParser::ReadToEndOfLine()
    {
        std::ostringstream ss;
        while(!IsNewline(PeekChar()))
        {
            const char c = ReadChar();
            if(c == 0)
            {
                return ss.str();
            }
            ss << c;
        }
        AdvanceChar();  // skip the newline
        return ss.str();
    }

    namespace  // local
    {
        bool
        IsSpaceCharacter(char c, bool include_newline)
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
    TextFileParser::SkipSpaces(bool include_newline)
    {
        while(IsSpaceCharacter(PeekChar(), include_newline))
        {
            ReadChar();
        }
    }

    bool
    TextFileParser::HasMore() const
    {
        return file->HasMore();
    }

    int
    TextFileParser::GetLine() const
    {
        return location.line;
    }

    int
    TextFileParser::GetColumn() const
    {
        return location.column;
    }

}  // namespace euphoria::core
