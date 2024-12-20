#pragma once


#include <ostream>

namespace eu::tests
{
    /// represents a error (false) or empty string (true)
    struct FalseString
    {
        std::string str;

        static FalseString create_false(const std::string& text);
        static FalseString create_true();

        operator bool() const;
    };

    std::ostream& operator<<(std::ostream& s, const FalseString& f);
}
