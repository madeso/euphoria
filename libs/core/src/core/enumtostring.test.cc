#include "core/enumtostring.h"

#include "tests/utils.h"
#include "tests/stringeq.h"

namespace euco = euphoria::core;

using namespace euphoria::tests;

namespace
{
    enum class MyEnum
    {
        X,
        Cat,
        ReallyLongValue
    };
}

TEST_CASE("enum2string", "[enum2string]")
{
    SECTION("single arg ToString")
    {
        CHECK(euco::enum_to_string(MyEnum::X) == "X");
        CHECK(euco::enum_to_string(MyEnum::Cat) == "Cat");
        CHECK(euco::enum_to_string(MyEnum::ReallyLongValue) == "ReallyLongValue");
    }

    SECTION("operator<<")
    {
        const auto e = MyEnum::Cat;

        std::stringstream ss;
        ss << e;

        CHECK(ss.str() == "Cat");
    }

    SECTION("no arg ToString")
    {
        CHECK(StringEq(
                euco::enum_to_string<MyEnum>(),
            {"X", "Cat", "ReallyLongValue"}));
    }

    SECTION("vector arg ToString")
    {
        const auto vec
                = std::vector<MyEnum> {MyEnum::Cat, MyEnum::X, MyEnum::Cat};
        CHECK(StringEq(
                euco::enum_to_string<MyEnum>(vec),
                {"Cat", "X", "Cat"}));
    }

    SECTION("match different cases")
    {
        auto       name  = GENERATE_AS(std::string, "cat", "CAT", "Cat", "cAt");
        const auto match = euco::string_to_enum<MyEnum>(name);
        CHECK(match.single_match);
        CHECK_THAT(match.values, CATCH_IS_VECTOR(MyEnum, MyEnum::Cat));
    }

    SECTION("match suggestions")
    {
        const auto match = euco::string_to_enum<MyEnum>("y", 2);
        CHECK_FALSE(match.single_match);
        CHECK_THAT(
                match.values, CATCH_IS_VECTOR(MyEnum, MyEnum::X, MyEnum::Cat));
    }
}
