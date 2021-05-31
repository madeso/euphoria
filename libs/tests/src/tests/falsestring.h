#pragma once

#include <string>
#include <ostream>

namespace euphoria::tests
{
    // represents a error (false) or empty string (true)
    struct false_string
    {
        static false_string
        create_false(const std::string& text);

        static false_string
        create_true();

        operator bool() const;
    
        std::string str;
    };

    std::ostream&
    operator<<(std::ostream& s, const false_string& f);
}
