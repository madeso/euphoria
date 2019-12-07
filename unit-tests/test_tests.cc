#include "stringeq.h"

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

        // size missmatch, 0-1
        CHECK_FALSE(StringEq(V{"dog"}, V{}));
        CHECK_FALSE(StringEq(V{}, V{"dog"}));

        // size missmatch 1-2, first is equal
        CHECK_FALSE(StringEq(V{"cat", "dog"}, V{"cat"}));
        CHECK_FALSE(StringEq(V{"cat"}, V{"cat", "dog"}));

        // size missmatch 1-2, first is not equal
        CHECK_FALSE(StringEq(V{"cat", "dog"}, V{"CAT"}));
        CHECK_FALSE(StringEq(V{"CAT"}, V{"cat", "dog"}));

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
