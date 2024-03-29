#include "base/enumtostring.h"

#include "tests/utils.h"
#include "tests/stringeq.h"

namespace core = eu;

using namespace eu::tests;

namespace
{
    enum class MyEnum
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
        CHECK(core::from_enum_to_string(MyEnum::x) == "x");
        CHECK(core::from_enum_to_string(MyEnum::cat) == "cat");
        CHECK(core::from_enum_to_string(MyEnum::really_long_value) == "really_long_value");
    }

    SECTION("formatting")
    {
        const auto e = MyEnum::cat;

        std::string ss = fmt::to_string(e);

        CHECK(ss == "cat");
    }

    SECTION("no arg to_string")
    {
        CHECK(is_string_equal(
                core::get_all_names_from_enum<MyEnum>(),
                {"x", "cat", "really_long_value"}));
    }

    SECTION("vector arg to_string")
    {
        const auto vec
                = std::vector<MyEnum> {MyEnum::cat, MyEnum::x, MyEnum::cat};
        CHECK(is_string_equal(
                core::from_enum_to_string<MyEnum>(vec),
                {"cat", "x", "cat"}));
    }

    SECTION("match different cases")
    {
        auto name = GENERATE_AS(std::string, "cat", "CAT", "Cat", "cAt");
        const auto match = core::from_string_to_enum<MyEnum>(name);
        CHECK(match.single_match);
        CHECK_THAT(match.values, CATCH_IS_VECTOR(MyEnum, MyEnum::cat));
    }

    SECTION("match suggestions")
    {
        const auto match = core::from_string_to_enum<MyEnum>("y", 2);
        CHECK_FALSE(match.single_match);
        CHECK_THAT(match.values, CATCH_IS_VECTOR(MyEnum, MyEnum::x, MyEnum::cat));
    }
}
