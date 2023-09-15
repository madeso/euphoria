#include "tests/stringeq.h"

#include "catch.hpp"

using namespace eu::tests;

TEST_CASE("tests-stringeq", "[tests]")
{
    SECTION("vector")
    {
        using V = std::vector<std::string>;
        CHECK(is_string_equal(V{}, V{}));
        CHECK(is_string_equal(V{"dog"}, V{"dog"}));
        CHECK(is_string_equal(V{"dog", "cat"}, V{"dog", "cat"}));

        // size missmatch, 0-1
        CHECK_FALSE(is_string_equal(V{"dog"}, V{}));
        CHECK_FALSE(is_string_equal(V{}, V{"dog"}));

        // size missmatch 1-2, first is equal
        CHECK_FALSE(is_string_equal(V{"cat", "dog"}, V{"cat"}));
        CHECK_FALSE(is_string_equal(V{"cat"}, V{"cat", "dog"}));

        // size missmatch 1-2, first is not equal
        CHECK_FALSE(is_string_equal(V{"cat", "dog"}, V{"CAT"}));
        CHECK_FALSE(is_string_equal(V{"CAT"}, V{"cat", "dog"}));

        CHECK_FALSE(is_string_equal(V{"dog"}, V{"doggo"}));
        CHECK_FALSE(is_string_equal(V{"dog", "cat"}, V{"dog", "catz"}));
    }

    SECTION("string")
    {
        CHECK(is_string_equal("", ""));
        CHECK(is_string_equal("dog", "dog"));

        CHECK_FALSE(is_string_equal("dog", "doggo"));
        CHECK_FALSE(is_string_equal("doggo", "dog"));
    }
}
