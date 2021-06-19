#include "core/enumtostring.h"

#include "tests/utils.h"
#include "tests/stringeq.h"

namespace euco = euphoria::core;

using namespace euphoria::tests;

namespace
{
    enum class my_enum
    {
        x,
        cat,
        really_long_value
    };
}

TEST_CASE("enum2string", "[enum2string]")
{
    SECTION("single arg to_string")
    {
        CHECK(euco::enum_to_string(my_enum::x) == "x");
        CHECK(euco::enum_to_string(my_enum::cat) == "cat");
        CHECK(euco::enum_to_string(my_enum::really_long_value) == "really_long_value");
    }

    SECTION("operator<<")
    {
        const auto e = my_enum::cat;

        std::stringstream ss;
        ss << e;

        CHECK(ss.str() == "cat");
    }

    SECTION("no arg to_string")
    {
        CHECK(string_is_equal(
                euco::enum_to_string<my_enum>(),
                {"x", "cat", "really_long_value"}));
    }

    SECTION("vector arg to_string")
    {
        const auto vec
                = std::vector<my_enum> {my_enum::cat, my_enum::x, my_enum::cat};
        CHECK(string_is_equal(
                euco::enum_to_string<my_enum>(vec),
                {"cat", "x", "cat"}));
    }

    SECTION("match different cases")
    {
        auto name = GENERATE_AS(std::string, "cat", "CAT", "Cat", "cAt");
        const auto match = euco::string_to_enum<my_enum>(name);
        CHECK(match.single_match);
        CHECK_THAT(match.values, CATCH_IS_VECTOR(my_enum, my_enum::cat));
    }

    SECTION("match suggestions")
    {
        const auto match = euco::string_to_enum<my_enum>("y", 2);
        CHECK_FALSE(match.single_match);
        CHECK_THAT(match.values, CATCH_IS_VECTOR(my_enum, my_enum::x, my_enum::cat));
    }
}
