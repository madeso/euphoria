#ifndef EUPHORIA_TESTS_STRINGEQ_H
#define EUPHORIA_TESTS_STRINGEQ_H

#include <vector>
#include <string>

#include "tests/falsestring.h"

namespace euphoria::tests
{
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
}  // namespace euphoria::tests

#endif  // EUPHORIA_TESTS_STRINGEQ_H
