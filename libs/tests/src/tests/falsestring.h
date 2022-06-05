#pragma once


#include <ostream>

namespace euphoria::tests
{
    // represents a error (false) or empty string (true)
    struct FalseString
    {
        static FalseString
        create_false(const std::string& text);

        static FalseString
        create_true();

        operator bool() const;
    
        std::string str;
    };

    std::ostream&
    operator<<(std::ostream& s, const FalseString& f);
}
