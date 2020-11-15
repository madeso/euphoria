#include "tests/falsestring.h"


namespace euphoria::tests
{
    FalseString FalseString::False(const std::string& text)
    {
        return {text};
    }


    FalseString
    FalseString::True()
    {
        return {""};
    }


    FalseString::operator bool() const
    {
        return str.empty();
    }


    std::ostream&
    operator<<(std::ostream& s, const FalseString& f)
    {
        if(f)
        {
            s << "<ok>";
        }
        else
        {
            s << f.str;
        }
        return s;
    }
}

