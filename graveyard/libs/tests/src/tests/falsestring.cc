#include "tests/falsestring.h"


namespace eu::tests
{
    FalseString FalseString::create_false(const std::string& text)
    {
        return {text};
    }


    FalseString
    FalseString::create_true()
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

