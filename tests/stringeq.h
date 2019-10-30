#ifndef EUPHORIA_TESTS_STRINGEQ_H
#define EUPHORIA_TESTS_STRINGEQ_H

#include <vector>
#include <string>

namespace euphoria::tests
{
    struct FalseString;

    /*
    
    Usage:
    
    using namespace euphoria::tests;
    
    CHECK(StringEq(foo(), "string"));
    CHECK(StringEq(bar(), {"some", "strings"}));

    */

    FalseString
    StringEq(const std::vector<std::string> lhs, const std::vector<std::string> rhs);

    FalseString
    StringEq(const std::string& lhs, const std::string& rhs);


    //
    // Implementation details:
    // 

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

}  // namespace euphoria::tests

#endif  // EUPHORIA_TESTS_STRINGEQ_H
