#ifndef EUPHORIA_TESTS_FALSE_STRING_H
#define EUPHORIA_TESTS_FALSE_STRING_H

#include <string>
#include <ostream>

namespace euphoria::tests
{
    // represents a error (false) or empty string (true)
    struct FalseString
    {
        static FalseString
        False(const std::string& text);

        static FalseString
        True();

        operator bool() const;
    
        std::string str;
    };

    std::ostream&
    operator<<(std::ostream& s, const FalseString& f);
}

#endif  // EUPHORIA_TESTS_FALSE_STRING_H

