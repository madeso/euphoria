#include "tests/stringeq.h"

#include "catch.hpp"

using namespace euphoria::tests;

TEST_CASE("tests-stringeq", "[tests]")
{
    CHECK(StringVecEq({}, {}));
    CHECK(StringVecEq({"dog"}, {"dog"}));
    CHECK(StringVecEq({"dog", "cat"}, {"dog", "cat"}));

    CHECK_FALSE(StringVecEq({"dog"}, {}));
    CHECK_FALSE(StringVecEq({}, {"dog"}));
    CHECK_FALSE(StringVecEq({"dog"}, {"doggo"}));
    CHECK_FALSE(StringVecEq({"dog", "cat"}, {"dog", "catz"}));
}
