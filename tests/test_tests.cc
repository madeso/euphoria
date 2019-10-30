#include "tests/stringeq.h"

#include "catch.hpp"

using namespace euphoria::tests;

TEST_CASE("tests-stringeq", "[tests]")
{
    CHECK(StringEq({}, {}));
    CHECK(StringEq({"dog"}, {"dog"}));
    CHECK(StringEq({"dog", "cat"}, {"dog", "cat"}));

    CHECK_FALSE(StringEq({"dog"}, {}));
    CHECK_FALSE(StringEq({}, {"dog"}));
    CHECK_FALSE(StringEq({"dog"}, {"doggo"}));
    CHECK_FALSE(StringEq({"dog", "cat"}, {"dog", "catz"}));
}
