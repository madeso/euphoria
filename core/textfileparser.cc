#include "core/textfileparser.h"

#include <sstream>

#include "core/numeric.h"

namespace euphoria::core
{
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
    }  // namespace

    TextFileParser::TextFileParser(const std::string& str)
        : string_(str)
        , length_(str.length())
        , position_(0)
        , line_(1)
        , column_(1)
    {}

    char
    TextFileParser::PeekChar(unsigned int advance)
    {
        const auto p = position_ + advance;
        if(p < length_)
        {
            return string_[p];
        }
        return 0;
    }


    std::string
    TextFileParser::PeekString(unsigned int advance)
    {
        const auto         c = PeekChar(advance);
        std::ostringstream ss;
        switch(c)
        {
        case 0: ss << "<null>"; break;
        case ' ': ss << "<space>"; break;
        case '\n': ss << "<newline>"; break;
        case '\t': ss << "<tab>"; break;
        default: ss << c; break;
        }
        return ss.str();
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
        const char r = PeekChar();

        if(IsNewline(r))
        {
            column_ = 1;
            line_ += 1;
        }
        else
        {
            column_ += 1;
        }

        ++position_;
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
            if(IsWithinInclusivei('a', c, 'z'))
            {
                return true;
            }
            if(IsWithinInclusivei('A', c, 'Z'))
            {
                return true;
            }
            if(IsWithinInclusivei('0', c, '9'))
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
    IsIdentStart(const char c)
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
        return position_ < length_;
    }

    unsigned int
    TextFileParser::GetLine()
    {
        return line_;
    }

    unsigned int
    TextFileParser::GetColumn()
    {
        return column_;
    }

}  // namespace euphoria::core
