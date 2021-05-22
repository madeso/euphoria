#include "tests/stringeq.h"

#include "catch.hpp"

using namespace euphoria::tests;

TEST_CASE("tests-stringeq", "[tests]")
{
    SECTION("vector")
    {
        using V = std::vector<std::string>;
        CHECK(string_is_equal(V{}, V{}));
        CHECK(string_is_equal(V{"dog"}, V{"dog"}));
        CHECK(string_is_equal(V{"dog", "cat"}, V{"dog", "cat"}));

        // size missmatch, 0-1
        CHECK_FALSE(string_is_equal(V{"dog"}, V{}));
        CHECK_FALSE(string_is_equal(V{}, V{"dog"}));

        // size missmatch 1-2, first is equal
        CHECK_FALSE(string_is_equal(V{"cat", "dog"}, V{"cat"}));
        CHECK_FALSE(string_is_equal(V{"cat"}, V{"cat", "dog"}));

        // size missmatch 1-2, first is not equal
        CHECK_FALSE(string_is_equal(V{"cat", "dog"}, V{"CAT"}));
        CHECK_FALSE(string_is_equal(V{"CAT"}, V{"cat", "dog"}));

        CHECK_FALSE(string_is_equal(V{"dog"}, V{"doggo"}));
        CHECK_FALSE(string_is_equal(V{"dog", "cat"}, V{"dog", "catz"}));
    }

    SECTION("string")
    {
        CHECK(string_is_equal("", ""));
        CHECK(string_is_equal("dog", "dog"));

        CHECK_FALSE(string_is_equal("dog", "doggo"));
        CHECK_FALSE(string_is_equal("doggo", "dog"));
    }
}
