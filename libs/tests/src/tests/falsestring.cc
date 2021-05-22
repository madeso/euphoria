#include "tests/falsestring.h"


namespace euphoria::tests
{
    false_string false_string::create_false(const std::string& text)
    {
        return {text};
    }


    false_string
    false_string::create_true()
    {
        return {""};
    }


    false_string::operator bool() const
    {
        return str.empty();
    }


    std::ostream&
    operator<<(std::ostream& s, const false_string& f)
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

