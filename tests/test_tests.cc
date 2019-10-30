#include "tests/stringeq.h"

#include "catch.hpp"

using namespace euphoria::tests;

TEST_CASE("tests-stringeq", "[tests]")
{
    SECTION("vector")
    {
        using V = std::vector<std::string>;
        CHECK(StringEq(V{}, V{}));
        CHECK(StringEq(V{"dog"}, V{"dog"}));
        CHECK(StringEq(V{"dog", "cat"}, V{"dog", "cat"}));

        CHECK_FALSE(StringEq(V{"dog"}, V{}));
        CHECK_FALSE(StringEq(V{}, V{"dog"}));
        CHECK_FALSE(StringEq(V{"dog"}, V{"doggo"}));
        CHECK_FALSE(StringEq(V{"dog", "cat"}, V{"dog", "catz"}));
    }

    SECTION("string")
    {
        CHECK(StringEq("", ""));
        CHECK(StringEq("dog", "dog"));

        CHECK_FALSE(StringEq("dog", "doggo"));
        CHECK_FALSE(StringEq("doggo", "dog"));
    }
}
