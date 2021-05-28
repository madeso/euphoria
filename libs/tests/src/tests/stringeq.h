#pragma once

#include <vector>
#include <string>

#include "tests/falsestring.h"

namespace euphoria::tests
{
    /*

    Usage:

    using namespace euphoria::tests;

    CHECK(string_is_equal(foo(), "string"));
    CHECK(string_is_equal(bar(), {"some", "strings"}));

    */

    false_string
    string_is_equal(const std::vector<std::string> lhs, const std::vector<std::string> rhs);

    false_string
    string_is_equal(const std::string& lhs, const std::string& rhs);
}
