#pragma once




#include "tests/falsestring.h"

namespace eu::tests
{
    /*

    Usage:

    using namespace eu::tests;

    CHECK(is_string_equal(foo(), "string"));
    CHECK(is_string_equal(bar(), {"some", "strings"}));

    */

    FalseString
    is_string_equal(const std::vector<std::string>& lhs, const std::vector<std::string>& rhs);

    FalseString
    is_string_equal(const std::string& lhs, const std::string& rhs);
}
