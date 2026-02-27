#pragma once

#include <string>
#include <optional>
#include <ostream>

namespace eu::tests
{
    /// represents a error (false) or empty string (true)
    struct FalseString
    {
        static
        FalseString
        False(const std::string& text);

        static
        FalseString
        True();

        bool
        IsTrue() const;

        operator bool() const;

        static
        FalseString
        Combine(const FalseString& lhs, const FalseString& rhs);
    
        std::optional<std::string> reason;
    };

    std::ostream&
    operator<<(std::ostream& s, const FalseString& f);
}
